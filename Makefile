sources = $(wildcard src/*.c)
includes = $(wildcard include/*.h)
objects = $(sources:.c=.o)
flags = -g -c -w -fPIC -std=c99 -luuid -ljson-c

build: $(objects)
	gcc $(objects) $(flags) -o ./bin/$(oyamo_exec)

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

clean:
	-rm -rf ./bin/*
	-rm -rf ./build/*
	-rm -rf ./src/*.o

generate-so:
	gcc $(objects) $(flags) -shared -o ./build/liboyamo.so -fPIC

liboyamo.a: $(objects)
	ar rcs ./build/$@ $^

install-dependencies:
	sudo apt-get install libjson-c-dev
	sudo apt-get install libuuid1

install:
	make install-dependencies
	make clean && make
	make liboyamo.a
	make uninstall
	sudo mkdir -p /usr/local/include/oyamo
	sudo cp $(includes) /usr/local/include/oyamo/
	sudo cp ./build/liboyamo.a /usr/local/lib/
uninstall:
	sudo rm -f -r /usr/local/include/oyamo
	sudo rm -f /usr/local/lib/liboyamo.a