#!/bin/bash

asd() {
cat <<"EOT"




                         .^.
                         /   \
                        /     \
                *******/       \*******
           ***** *****/         \***** *****
       ***** ********/           \******** *****
      *** **********/             \********** ***
       ***** ******/               \****** *****
           ***** */        _**_     \* *****
                */      _-******\    \*
                /    _-" *****   "\   \
                \__-"              "\_/



EOT
}

asd


echo -e "***** Setting up  eMonitor server *****"
sudo cp emonitor /etc/init.d/emonitor
sudo chmod 0755 /etc/init.d/emonitor
sudo update-rc.d emonitor defaults

mkdir /home/$USER/Documents
sudo chmod 0755 /home/$USER/Documents

mkdir /home/$USER/Documents/bot
sudo chmod 0755 /home/$USER/Documents/emonitor

mkdir /home/$USER/Documents/log/
sudo chmod 0755 /home/$USER/Documents/log/


cd /home/$USER/eMonitor/server/app
npm install

exit 0
