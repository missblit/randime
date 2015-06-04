CXXFLAGS = -std=c++14 -Wall -Wextra -pthread -g
LDFLAGS = -lboost_system -lboost_coroutine -lstdc++ -lcurl -lgumbo -pthread
.PHONY: rapunzel
all: scraper_main server_main
rapunzel/rapunzel.a: 
	cd rapunzel && make
server_main: rapunzel/rapunzel.a 
scraper_main: randime_scraper.o gumbo_util.o
scraper_main.o: randime_show.h randime_scraper.h
server_main.o: randime_show.h
gumbo_util.o: gumbo_util.h
randime_scraper.o: randime_scraper.h randime_show.h
