def decode(a,b):
	high = (a-128)<<7;
	low = (b-128);
	return high + low;

def main():
	print( "recovered ", decode(255,255) ) # 2723 encode

if __name__ == "__main__":
	main()