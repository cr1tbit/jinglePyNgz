import turbopinger,ipaddress
import time

addr=ipaddress.ip_address('::1')

ip_list = []

# This library is made for drawing images via ipv6, not DDOSing.
# The images should never exceed 2M pixels, so allocating everything
# at once is fine and makes (re)drawing faster.
# If you set spam_count to 50M, the app will allocate ~4GB of ram
spam_count = 5*1000*1000

for i in range (spam_count):
    ip_list.append(addr.packed)

print("now pinging "+ str(len(ip_list))+ "x...")

start = time.time()

turbopinger.blast_ip6_list(ip_list)

end = time.time()

time = end-start
print("effeciency: {sps} pings/sec.".format(sps=spam_count/time))

print(end - start)

