import time
import math as m

start = time.time()


def hex_parser():

	temp, data, block = [], [], []
	x, addr, i = 0, 0, 1
	
	# open the file, break lines at '\n'
	with open("main.hex", "r") as fp:
		hex_line = fp.read()
		lines = hex_line.splitlines()         
	
	# extract data from each line
	for line in lines:
		data.append(line[9:][:-2])

	# form a word by combining two bytes
	for code in data:
		for x in range(0,(len(code) - 1), 2):
			p  = code[x : x + 2]
			block.append(p)

	# append extra 'FF's to get the required size of 128 per page
	while((len(block) % 128) != 0):
		block.append('FF');

	count = int(len(block) / 128)

	print("Start Flashing")

	while(i <= count):

		l_addr = hex(int(addr % 256))
		h_addr = hex(int(addr / 256))
		addr += 64

		# Intitialize flashing
		print("Loading Page Address....")
		print("0x55", h_addr, l_addr, "0x20")
		time.sleep(1)

		# Add header, add 128 words and send one-by-one
		print("Flashing Block: ", i, "H_ADDR: ", h_addr, "L_ADDR: ", l_addr)
		print("0x64", "0x00", "0x80", "0x46")
		time.sleep(1)

		# for j in range(0,128):
		# 	print(("0x" + block[j]))
		
		print("BLOCK")
		print("0x20")

		block = block[128:]
		i += 1
		time.sleep(1)

	print("Done Flashing")

hex_parser()

print('Time (ms):', 1000*(time.time() - start))
