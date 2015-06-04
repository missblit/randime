scraper:
	g++ -std=c++14 scraper_main.cpp gumbo_util.cpp randime_scraper.cpp -lcurl -lgumbo -g -Wall -Wextra 
server:
	g++ -std=c++14 server_main.cpp rapunzel/fcgi_connection_manager.cpp rapunzel/fcgi_request.cpp rapunzel/fcgi_connection.cpp \
            -g -Wall -Wextra -lcurl -lboost_system -pthread
