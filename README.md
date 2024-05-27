# DioWType
A small command line program for Wayland to simulate a key press.
Similar to ydotool but it doesn't type text, it only simulates one single key press.
You can simulate any key press, for instance to simulate Enter (return) key press you run: diowtype 28
It doesn't need any uinput or background deamons, it's simple small and one-shot application.
In order to find which number is attributed to which key (for instance the Enter key) you need to open a terminal and run:

		cat /usr/include/linux/input-event-codes.h | grep -i enter

you will get the outout:

		#define KEY_ENTER		28

so there you have it, it's 28, hence: diowtype 28 will press and release the Enter key.

# What you can do with DioWType
   1. Simulate any key press.

   to build the project you need to install the following libs:

		make
		pkgconf
		libwayland-dev
		libxkbcommon-dev

   on Debian run the following command:

		sudo apt install make pkgconf libwayland-dev libxkbcommon-dev

# Installation/Usage
  1. Open a terminal and run:
 
		 chmod +x ./configure
		 ./configure

  2. if all went well then run:

		 make
		 sudo make install
		 
		 (if you just want to test it then run: make run)

  3. Run the application (for instance pressing lowercase letter 'a'):
  
		 diowtype 30

# Support

   My Libera IRC support channel: #linuxfriends

   Matrix: https://matrix.to/#/#linuxfriends2:matrix.org

   Email: nicolas.dio@protonmail.com
