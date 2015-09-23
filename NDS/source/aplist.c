#include <nds.h>
#include <dswifi9.h>
#include <time.h>
#include "aplist.h"


#define TIMEOUT	10		// entry is removed after TIMEOUT refreshes

const aplist* getAp(const aplist *head, unsigned char mac[6]);
void updateAp(aplist *cur, const Wifi_AccessPoint *ap);
void insertAp(aplist **head, const Wifi_AccessPoint *ap);


void updateApList(aplist **head, unsigned short filter, APCALLBACK* added, APCALLBACK* removed)
{
	aplist *cur;
	aplist *temp;
	unsigned int i;
	Wifi_AccessPoint ap;
	
	// update list and add new access points
	for (i=0; i<Wifi_GetNumAP(); i++) {
		if (Wifi_GetAPData(i, &ap) != WIFI_RETURN_OK) {
			continue;
		}
		if (ap.flags & filter) {
			continue;
		}
		// check if already in list
		cur = (aplist*)getAp(*head, ap.macaddr);
		if (cur) {
			updateAp(cur, &ap);
		} else {
			insertAp(head, &ap);
			if (added) {
				(added)(*head);
			}
		}
	}
	
	// remove old access points
	cur = *head;
	while (cur) {
		cur->timeout++;
		if (cur->timeout > TIMEOUT) {
			if (cur->prev) {
				cur->prev->next = cur->next;
			} else {
				*head = cur->next;
			}
			if (cur->next) {
				cur->next->prev = cur->prev;
			}
			temp = cur->next;
			// callback
			cur->prev = NULL;
			cur->next = NULL;
			if (removed) {
					(removed)(cur);
			}
			free(cur);
			cur = temp;
		} else {
			cur = cur->next;
		}
	}
}


void sortApList(aplist **head, bool asc)
{
	aplist *cur = *head;
	aplist *temp;
	
	while (cur) {
		if (cur->next && ((asc && cur->rssi > cur->next->rssi) || (!asc && cur->rssi < cur->next->rssi))) {
			// switch entry position
			if (cur->prev) {
				cur->prev->next = cur->next;
			} else {
				*head = cur->next;
			}
			cur->next->prev = cur->prev;
			temp = cur->next->next;
			cur->next->next = cur;
			cur->prev = cur->next;
			cur->next = temp;
			if (temp) {
				temp->prev = cur;
			}
			// reset to head
			cur = *head;
		} else {
			cur = cur->next;
		}
	}
}


void freeApList(aplist **head, APCALLBACK* removed)
{
	aplist *cur = *head;
	aplist *temp;
	
	while (cur) {
		temp = cur->next;
		cur->prev = NULL;
		cur->next = NULL;
		if (removed) {
			(removed)(cur);
		}
		free(cur);
		cur = temp;
	}
	
	*head = NULL;
}


const aplist* getAp(const aplist *head, unsigned char mac[6])
{
	const aplist *cur = head;
	
	while (cur) {
		if (!memcmp(cur->mac, mac, 6)) {
			break;
		}
		cur = cur->next;
	}
	
	return cur;
}


void updateAp(aplist *cur, const Wifi_AccessPoint *ap)
{
	// update rssi information, reset timeout
	cur->rssi = ap->rssi;
	if (ap->rssi > cur->max_rssi) {
		cur->max_rssi = ap->rssi;
	}
	if (ap->rssi < cur->min_rssi) {
		cur->min_rssi = ap->rssi;
	}
	cur->sum_rssi += ap->rssi;
	cur->num_rssi++;
	cur->timeout = 0;
}


void insertAp(aplist **head, const Wifi_AccessPoint *ap)
{
	aplist *cur;
	
	// add to beginning of list
	cur = (aplist*)malloc(sizeof(aplist));
	if (!cur) {
		return;			// error: OOM
	}
	cur->channel = ap->channel;
	cur->flags = ap->flags;
	memcpy(cur->mac, ap->macaddr, 6);
	cur->rssi = ap->rssi;
	cur->max_rssi = ap->rssi;
	cur->min_rssi = ap->rssi;
	cur->sum_rssi = ap->rssi;
	cur->num_rssi = 1;
	memcpy(cur->ssid, ap->ssid, ap->ssid_len);
	// make string NULL-terminated
	*(cur->ssid+ap->ssid_len) = '\0';
	cur->firstseen = time(NULL);
	cur->first_lat = 0;
	cur->first_long = 0;
	cur->timeout = 0;
	
	cur->prev = NULL;
	if (*head) {
		cur->next = *head;
		(*head)->prev = cur;
	} else {
		cur->next = NULL;
	}
	*head = cur;
}
