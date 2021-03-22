all:server client

server:../es_udp_multiplexing.cpp ../es_udp_test_server.cpp
	g++ ../es_udp_multiplexing.cpp -std=c++17 -c;
	g++ ../es_udp_test_server.cpp -c -std=c++17;
	g++ es_udp_test_server.o es_udp_multiplexing.o -o server.out;

client:../es_udp_test_client.cpp
	g++ -std=c++17 ../es_udp_test_client.cpp -o client.out;
