/*
 * Copyright (C) 2012 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <cutils/properties.h>

/* Read properties and set it to other wanted formats */

int main() {
	int fd1; FILE *fd;
	int macbyte;
	int i;
	struct termios  ios;
	char sync_buf[256];
	char *readbuf = sync_buf;
        char mSwVer[PROPERTY_VALUE_MAX];
        char *mMacAddr;
	int old_flags;
	int modem=0;
	int retries=5;
	int read_bytes=-1;

        property_get("lge.version.sw",mSwVer,"Unknown");
        property_set("gsm.version.baseband",mSwVer);

        /* Now for the wifistuff */
	modem=open("/dev/smd0",O_RDWR);
	if (modem<=0) {
		return 1;
	}
	tcgetattr( modem, &ios );
	ios.c_lflag = 0;
	tcsetattr( modem, TCSANOW, &ios );
	old_flags = fcntl(modem, F_GETFL, 0);
	fcntl(modem, F_SETFL, old_flags | O_NONBLOCK);
	while (read_bytes < 0 && retries) {
		read_bytes = write(modem,"AT%MAC\r",7);
		sleep(1);
		retries--;
	}

	if (read_bytes > 0) {
		retries = 5;
		read_bytes = -1;
	}

	while (read_bytes < 0 && retries) {
		read_bytes = read(modem,sync_buf,sizeof(sync_buf));
		sleep(1);
		retries--;
	}
		
	if (read_bytes > 0) {
		/* Skip first echoed line */
		while (read_bytes && *readbuf != '\n' && *readbuf !='\0') {
			readbuf++;
			read_bytes--;
		}
		/* Nothing left */
		if (!read_bytes) { return 2; }

		/* Skip line break */
		readbuf++;
		read_bytes--;

		/* Nothing left */
		if (!read_bytes) { return 2; }

		mMacAddr = readbuf;

		while (*readbuf != '\r' && *readbuf != '\n' && *readbuf !='\0' && read_bytes) {
			readbuf++;
			read_bytes--;
		}
		*readbuf='\0';
		close(modem);

		/* Do we have something ? */
		if (strlen(mMacAddr)>2) {
			/* Chop off first and last chars */
			*mMacAddr++;
			*--readbuf='\0';
                }


		if (strlen(mMacAddr)==12) {
			fd = fopen("/data/misc/wifi/WCN1314_qcom_cfg.ini","w");
			fprintf(fd,"\
gEnableImps=1\n\
gEnableIdleScan=0 \n\
gImpsModSleepTime=600  \n\
gEnableBmps=1\n\
gEnableSuspend=3\n\
gDot11Mode=0\n\
gEnableHandoff=0\n\
gRoamingTime=0 \n\
NetworkAddress=F80CF35EA114\n\
InfraUapsdVoSrvIntv=0 \n\
InfraUapsdViSrvIntv=0\n\
InfraUapsdBeSrvIntv=0\n\
InfraUapsdBkSrvIntv=0 \n\
DelayedTriggerFrmInt=18000\n\
gEnableFWRssiMonitoring=0  \n\
gNumRxAnt=1  \n\
gNthBeaconFilter=50  \n\
McastBcastFilter=3\n\
hostArpOffload=1\n\
gEnableLogp=1\n\
gAPMacAddr=%s\n\
gEnableApProt=1\n\
gEnableApOBSSProt=0\n\
gEnableApUapsd=1\n\
gFixedRate=0\n\
RTSThreshold=2347\n\
gDisableIntraBssFwd=0\n\
WmmIsEnabled=0\n\
g11dSupportEnabled=1\n\
gShortGI20Mhz=1  \n\
gAPAutoShutOff=0\n\
gEnablePhyAgcListenMode=128\n\
gDataInactivityTimeout=200\n\
gBmpsModListenInterval = 65535\n\
END\n\
",mMacAddr);
			fclose(fd);
		}

	}
	return 0;
}
