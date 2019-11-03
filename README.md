## jinglePyNgz - no ddos, only draw

Python wrapper library around turbofast jinglePings pinger, that has been written in pure C. Hopefully more customisable. Thanks to python and stuff.

### What is jinglePings?

An online event, where publically accesable billboard may be drawn on, pixel-by-pixel, by pinging specific IP6 address, with corresponding X,Y cords, and RGBA value.

### How does this module work?

The `turbopinger` module written in C makes use of direct socket access mode, so that it can bypass the OS layer, and form the IMCP6 packet directly. It doesn't care for the response too.

Because of that "raw" mode, the app requires to be run as `sudo`.

The `turbopinger` module may be installed by command:

`sudo python setup.py install`

After that, take a look at some examples, which may be located in the `examples/` folder.

### Has it been tested?

Well, locally yes. I managed to completely nuke my raspberry pi with it.

Regarding usage with jingePings board, I managed to spew out an proof-of-concept python app 5 minutes between this year's November event has ended. Did not manage to notice the results. So, let's wait till Christmas I guess... :(
