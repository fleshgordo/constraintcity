//
// dswifi access point list
//


struct _aplist
{
	unsigned char 		channel;
	unsigned short 		flags;		// see dswifi9.h (WEP .. 0x02, WPA .. 0x04)
	unsigned char 		mac[6];
	unsigned char 		rssi;		// signal strength
	unsigned char		max_rssi;	// maximal rssi
	unsigned char		min_rssi;	// minimal rssi
	unsigned long 		sum_rssi;	// sum of all rssi readings
	unsigned long 		num_rssi;	// number of rssi readings
	char 			ssid[34];
	time_t			firstseen;	// unix timestamp
	float			first_lat;	// latitude
	float			first_long;	// longitude
	unsigned short 		timeout;	// 0 .. TIMEOUT
	struct _aplist 		*prev;
	struct _aplist 		*next;
};
typedef struct _aplist aplist;

typedef void (APCALLBACK)(aplist*);


void updateApList(aplist **head, unsigned short filter, APCALLBACK* added, APCALLBACK* removed);
// you can remove certain access points by passing a flags bitmap as filter
// provide the address of callback functions or NULL

void sortApList(aplist **head, bool asc);
// sorts an access point list by rssi

void freeApList(aplist **head, APCALLBACK* removed);
