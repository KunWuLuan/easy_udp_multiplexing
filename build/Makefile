all:server client

server:
    g++ ../es_udp_multiplexing.cpp -std=c++17 -c
    g++ ../es_udp_test_server.cpp -c -std=c++17
    g++ es_udp_test_server.o es_udp_multiplexing.o -o server.out

client:
    g++ -std=c++17 ../es_udp_test_client.cpp -o client.out
