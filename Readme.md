To run these codes successfully, you need to compile and run them in 2 separate terminal windows simultaneously.
also you need to run, the server before the client. 
Store the 2 files in 2 different folders to properly see the upload and download.
store files to be downloaded in the same directory / folder as the server .
store files to be uploaded in the same directory / folder as the client.

To run this the server code, 
	open a new terminal window 
	change directory to the path the file is found in using,
		cd path/to/server/file.

in the directory, type
	g++ -std=c++11 -o server server .cpp 
		this compiles and creates an exec file called server 
		
	after compilation, type
		./server - this runs the compiled file. 
		
To run this the client code, 
	open a new terminal window 
	change directory to the path the file is found in using,
		cd path/to/client/file.

in the directory, type
	g++ -std=c++11 -o client client .cpp 
		this compiles and creates an exec file called client 
		
	after compilation, type
		./client - this runs the compiled file. 
		
when you run, you will be given the option to either upload or download a file. 
to upload simply type in 1, to download type in 2
the file to upload / download have been specified in the code already, 
to change it simply go to the client code:
	const char* uploadFilename = "upload_file.txt";
	const char* downloadFilename = "download_file.txt";
	
	and change "upload_file.txt" or "download_file.txt" to the name of the file to upload or download.
	
This code uses loopback address to run on different devices, configure their IP address
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    chang "127.0.0.1" to specified IP address.
    
To change the port number 
    serverAddr.sin_port = htons(8080); change '8080' to your designed port number for both server and client.