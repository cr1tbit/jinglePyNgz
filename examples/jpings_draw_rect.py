import turbopinger,ipaddress

import time

# format: 2001:610:1908:a000:<X>:<Y>:<B><G>:<R><A>

jpings_addr_prefix="2001:610:1908:a000"

ip_list = []

for x in range (100,150):
    for y in range(100,150):
        ip_list.append(
            ipaddress.ip_address(
                f"{jpings_addr_prefix}:{x}:{y}:DDDD:DDDD"
                ).packed
        )


print(len(ip_list))

start = time.time()

#print(ip_list)
turbopinger.blast_ip6_list(ip_list)

end = time.time()

time = end-start
print("effeciency: {sps} pings/sec.".format(sps=len(ip_list)/time))

print(end - start)

