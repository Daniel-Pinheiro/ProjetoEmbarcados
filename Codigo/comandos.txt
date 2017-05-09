
alias python=python3

sudo su

apt-get update

apt-get install python3 python3-pip libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0

alias pip=pip3

pip install --upgrade pip

pip install pyaudio testresources speechrecognition
