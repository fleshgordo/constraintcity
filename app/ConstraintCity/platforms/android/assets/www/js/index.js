/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
var app = {

	/* ----------------------------------------------------------------------------------- */
	/* VARIABLES */
	/* ----------------------------------------------------------------------------------- */
	isWifiOn: false,
	isBluetoothOn: false,
	WifiInterval: 0,
	BluetoothInterval: 0,
	WifiList: {},
	settings: {
		bluetoothInterval: 2000,
		wifiInterval: 2000,
		motorMultiplier: 1,
		minMultiplier: 0,
		maxMultiplier: 15,
		MacAddress: "20:13:05:15:04:57"
	},

	/* ----------------------------------------------------------------------------------- */
	/* INIT */
	/* ----------------------------------------------------------------------------------- */

	/**
	 * Application Constructor
	 */

	initialize: function() {
		this.bindEvents();
	},

	/**
	 * Bind Event Listeners
	 * Bind any events that are required on startup. Common events are:
	 * 'load', 'deviceready', 'offline', and 'online'.
	 */
	bindEvents: function() {
		document.addEventListener('deviceready', this.onDeviceReady, false);
	},

	/**
	 * Initializes the setting menu and binds the menubutton handler
	 */
	initializeMenu: function() {
		document.addEventListener("menubutton", function() {
			$('.menuSettings').slideToggle();
			$('body').append('<div class="shadow"></div>');
			$('.shadow').on('click', function() {
				$('.menuSettings').slideToggle();
				$(this).remove();
			});
		}, false);

		$(".buttonMotorsActivate").one("click", app.bluetooth.engageMotors);

		$('.bluetoothStatus').on('click', function() {
			bluetoothSerial.disconnect(function() {
				console.log('*** bluetooth disconnecting success');
			}, function() {
				console.log('*** bluetooth disconnecting failed');
			});
			bluetoothSerial.connect(app.settings.MacAddress, app.bluetooth.isConnected, app.bluetooth.failedConnection);
		});

		$('.buttonMotorsTest').on('click', function() {
			bluetoothSerial.write('t', function() {
				console.log('writing char t to test motors');
			}, function() {
				console.log('failed to write t to test motors');
			});
		});

		$('.buttonMorePain').on('click', function() {
			bluetoothSerial.write('m', function() {
				if (app.settings.motorMultiplier < app.settings.maxMultiplier) {
					app.settings.motorMultiplier += 1;
					$('div.motorMultiplier').html(app.settings.motorMultiplier);
				}

				console.log('writing char m to increase pain');
			}, function() {
				console.log('failed to write m to increase pain');
			});
		});

		$('.buttonLessPain').on('click', function() {
			bluetoothSerial.write('l', function() {
				if (app.settings.motorMultiplier > app.settings.minMultiplier) {
					app.settings.motorMultiplier -= 1;
					$('div.motorMultiplier').html(app.settings.motorMultiplier);
				}
				console.log('writing char l to lower pain');
			}, function() {
				console.log('failed to write l to lower pain');
			});
		});

		$('.buttonMotorsCheck').on('click', function() {
			bluetoothSerial.write('c', function() {
				console.log('writing char c to check motors');

			}, function() {
				console.log('failed to write c to check motors');
			});
		});
	},

	/**
	 * Deviceready Event Handler
	 * The scope of 'this' is the event. In order to call the 'receivedEvent'
	 * function, we must explicitly call 'app.receivedEvent(...);'
	 */
	onDeviceReady: function() {
		// start the wifi scan in timed interval
		app.WifiInterval = setInterval(function() {
			WifiWizard.startScan(app.wifi.wifiScanStarted, app.wifi.wifiScanFailed);
			WifiWizard.getScanResults(app.wifi.handleResults, app.wifi.handleResultsFailed);
		}, app.settings.wifiInterval);

		app.initializeMenu();

		// check if Bluetooth is on:
		bluetoothSerial.isEnabled(
			app.bluetooth.listPorts,
			app.bluetooth.notEnabled
		);
	},

	/* ----------------------------------------------------------------------------------- */
	/* Bluetooth related stuff */
	/* ----------------------------------------------------------------------------------- */


	bluetooth: {
		/**
		 * List available bluetooth port (called through bluetoothSerial callback)
		 */
		listPorts: function() {
			app.isBluetoothOn = true;
			bluetoothSerial.connect(app.settings.MacAddress, app.bluetooth.isConnected, app.bluetooth.failedConnection);
			console.log('*** bluetooth: ... starting bluetooth scan');
			// bluetoothSerial.list(app.bluetooth.listDevices, function() {
			// 	console.log('device listing failed');
			// });
		},

		listDevices: function(deviceList) {
			console.log(deviceList);
		},

		/**
		 * Bluetooth connection is established
		 */
		isConnected: function() {
			app.bluetooth.setBluetoothStatus(true);
			$('.header').on('click', function() {
				if (app.isBluetoothOn === true) {
					app.bluetooth.sendWifiList();
				}
			});
			setInterval(function() {
				bluetoothSerial.isConnected(function() {
					app.bluetooth.setBluetoothStatus(true)
				}, function() {
					app.bluetooth.setBluetoothStatus(false)
				});
			}, 10000);
		},

		/**
		 * Set the bluetooth connection tab active/disactive
		 */
		setBluetoothStatus: function(_connected) {
			console.log('checking bluetooth status');
			if (_connected == true) {
				$('.bluetoothStatus').removeClass('bluetoothInactive').addClass('bluetoothActive');
			} else {
				$('.bluetoothStatus').removeClass('bluetoothActive').addClass('bluetoothInactive');
			}
		},


		/**
		 * Writing to serial port success
		 */
		writeSuccess: function() {
			console.log('*** bluetooth writing to serial port [ok]');
		},

		/**
		 * Reading from serial port failed
		 */
		readFailure: function() {
			console.log('*** bluetooth reading from serial port [failed]');
		},

		/**
		 * Writing to serial port failed
		 */
		writeFailure: function() {
			app.bluetooth.setBluetoothStatus(true);
			console.log('*** bluetooth writing to serial port [failed]');
		},

		/**
		 * Bluetooth connection failed
		 */
		failedConnection: function() {
			console.log('*** bluetooth failed ... try reconnecting ...');
		},

		/**
		 * Bluetooth is not switched on (called through bluetoothSerial callback)
		 */
		notEnabled: function() {
			console.log('*** bluetooth: please switch on');
			bluetoothSerial.enable(function() {
				console.log('*** bluetooth: user has switched on')
				app.isBluetoothOn = true;
				$('.bluetoothStatus').removeClass('bluetoothInactive').addClass('bluetoothActive');
			}, function() {
				console.log('*** bluetooth: user has refused to switch on via prompt');
			});
		},

		/**
		 * Sends the the first three wifi ESSID's as int array via bluetooth
		 */
		sendWifiList: function() {
			var wifiListSorted = [];
			for (var i = 0; i < 3; i++) {
				var level = 0;
				try {
					level = (100 + +app.WifiList[i]['level']);
				} catch (err) {

				}
				var motorID = i + 1;
				wifiListSorted[i] = +level;
				$('#motor' + motorID + 'Pos').html(+level);
			}
			console.log('***  sending bluetooth signal');
			console.log(wifiListSorted);
			bluetoothSerial.write(wifiListSorted, app.bluetooth.writeSuccess, app.bluetooth.writeFailure);
			bluetoothSerial.write('\n', app.bluetooth.writeSuccess, app.bluetooth.writeFailure);
		},

		/**
		 * Sends character c via bluetooth to receive status report from dynamixel motors
		 */
		checkMotors: function() {
			bluetoothSerial.write('c', function() {
				console.log('*** bluetooth: check dynamixel');
				bluetoothSerial.readUntil('\n', function(data) {
					console.log('*** bluetooth: reading data from motors: ');
					console.log(data);
					$('span.motorStatus').html(data);
				}, function() {
					console.log('*** bluetooth: failed to read data from motors');
				});
			}, function() {
				console.log('*** bluetooth: cannot send data via bluetooth');
			});
		},

		/**
		 * Sends character p via bluetooth followed by pain mutliplier
		 */

		sendPainMultiplier: function(painFactor) {

			var send = 'p' + painFactor;
			bluetoothSerial.write(send, function() {
				console.log('writing p to serial port');
			}, function() {
				console.log('failed to write p' + painFactor + ' to serial port');
			});
			bluetoothSerial.write(+painFactor, function() {
				console.log('writing ' + painFactor + ' to serial port');
			}, function() {
				console.log('failed to write p' + painFactor + ' to serial port');
			});
			//bluetoothSerial.write(+painFactor, function() {}, function() {});
			//bluetoothSerial.write('\n', function() {}, function() {});
		},

		/**
		 * Sends character a via bluetooth to engage the motors
		 */
		engageMotors: function() {
			$(this).one("click", app.bluetooth.disengageMotors);
			if (app.isBluetoothOn === true) {
				bluetoothSerial.write('a', function() {
					console.log('*** bluetooth: activate motors');
					$(this).html('Disactive motors');
					app.BluetoothInterval = setInterval(function() {
						app.bluetooth.sendWifiList();
					}, app.settings.bluetoothInterval);
				}, function() {
					console.log('*** bluetooth: error sending activate motor signal');
				});
				$(this).html('Disactivate motors');
			}
		},

		/**
		 * Sends character d via bluetooth to disactivate the motors
		 */
		disengageMotors: function() {
			$(this).one("click", app.bluetooth.engageMotors);
			if (app.isBluetoothOn === true) {
				bluetoothSerial.write('d', function() {
					console.log('*** bluetooth: disactivate motors');
					clearInterval(app.BluetoothInterval);
				}, function() {
					console.log('*** bluetooth: error sending disactivate motor signal');
				});
				$(this).html('Activate motors');
			}
		},
	},

	/* ----------------------------------------------------------------------------------- */
	/* WIFI related stuff */
	/* ----------------------------------------------------------------------------------- */

	wifi: {

		/**
		 * Success function Wifi scan is being started
		 * @param {string} - response   ok when everything's fine
		 */
		wifiScanStarted: function(response) {
			console.log('### WIFI: scanning started: ' + response);
		},

		/**
		 * Order object array by signal strength level
		 * @param {_list} - _list   array object with accesspoints
		 */
		handleResults: function(_list) {
			_list.sort(function(a, b) {
				if (a.level > b.level) {
					return -1;
				}
				if (a.level < b.level) {
					return 1;
				}
				// a must be equal to b
				return 0;
			});

			//app.WifiList = _list;
			// rendering results into HTML
			app.wifi.renderResults(_list);
		},

		/**
		 * Renders the a list of accesspoints into html
		 * @param {object} - _list   array object with accesspoints
		 */
		renderResults: function(_list) {
			var contentSSID = '',
				contentLevels = '',
				contentChannel = '',
				contentEncryption = '',
				j = 0;
			app.WifiList = {};
			for (var i = 0; i < _list.length; i++) {
				// check if wifi closed or open 
				if (_list[i]['capabilities'].indexOf('WPA') != -1 || _list[i]['capabilities'].indexOf('WEP') != -1) {
					app.WifiList[j] = _list[i];
					j += 1;
					if (_list[i]['capabilities'].indexOf('WPA') != -1) {
						contentEncryption += 'WPA' + '<br>';
					} else {
						contentEncryption += 'WEP' + '<br>';
					}
					if (_list[i]['SSID'] == '') {
						contentSSID += '--Hidden--';
					} else {
						contentSSID += _list[i]['SSID'] + '<br>';
					}
					contentLevels += _list[i]['level'] + '<br>';
					contentChannel += _list[i]['frequency'] + '<br>';
				}
			}
			$('.SSIDList').html(contentSSID);
			$('.WifiLevelList').html(contentLevels);
			$('.WifiChannelList').html(contentChannel);
			$('.WifiEncryptionList').html(contentEncryption);
		},

		/**
		 * Shows failure of results
		 */
		handleResultsFailed: function(_e) {
			console.log('### WIFI: failed error: ' + _e);
		},

		/**
		 * Shows failure when WIFI scan fails
		 */
		wifiScanFailed: function() {
			console.log('### WIFI: scanning failed');
		},
	},

	/* ----------------------------------------------------------------------------------- */
	/* CONNECTION */
	/* ----------------------------------------------------------------------------------- */

	/**
	 * Check the device's connection status
	 * @return {string} - Connection state as string	 
	 */
	checkConnection: function() {
		var networkState = navigator.connection.type;

		var states = {};
		states[Connection.UNKNOWN] = 'Unknown connection';
		states[Connection.ETHERNET] = 'Ethernet connection';
		states[Connection.WIFI] = 'WiFi connection';
		states[Connection.CELL_2G] = 'Cell 2G connection';
		states[Connection.CELL_3G] = 'Cell 3G connection';
		states[Connection.CELL_4G] = 'Cell 4G connection';
		states[Connection.CELL] = 'Cell generic connection';
		states[Connection.NONE] = 'No network connection';

		return 'Connection type: ' + states[networkState];
	}


};