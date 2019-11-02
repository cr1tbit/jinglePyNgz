#!/bin/bash

PYPATH=$(python3 -c "import sysconfig; print(sysconfig.get_path('include'))")

gcc -DSTANDALONE -I$PYPATH -o turbopinger -O3 -Wall turbopinger.c
sudo ./turbopinger
