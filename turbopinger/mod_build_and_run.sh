sudo python setup.py install
sudo python -c "\
import turbopinger,ipaddress;\
addr=ipaddress.ip_address('::1');\
turbopinger.blast_ip6_list([addr.packed,addr.packed]);"
