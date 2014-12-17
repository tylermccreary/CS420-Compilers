token.o: token.h token.cc
	g++ -c token.cc
addop.o: token.h addoptoken.h addoptoken.cc
	g++ -c addoptoken.cc token.cc
idtoken.o: token.h idtoken.h idtoken.cc
	g++ -c idtoken.cc token.cc
eoftoken.o: token.h eoftoken.h eoftoken.cc
	g++ -c eoftoken.cc token.cc
muloptoken.o: token.h muloptoken.h muloptoken.cc
	g++ -c muloptoken.cc token.cc
numtoken.o: token.h numtoken.h numtoken.cc
	g++ -c numtoken.cc token.cc
punctoken.o: token.h punctoken.h punctoken.cc
	g++ -c punctoken.cc token.cc
reloptoken.o: token.h reloptoken.h reloptoken.cc
	g++ -c reloptoken.cc token.cc
keywordtoken.o: token.h keywordtoken.h keywordtoken.cc
	g++ -c keywordtoken.cc token.cc
buffer.o: buffer.h buffer.cc
	g++ -c buffer.cc
scanner.o: scanner.h scanner.cc
	g++ -c scanner.cc
parser.o: parser.h parser.cc
	g++ -c parser.cc
symbol_table.o: symbol_table.h symbol_table.cc
	g++ -c symbol_table.cc
register.o: register.h register.cc
	g++ -c register.cc
register_allocator.o: register_allocator.h register_allocator.cc
	g++ -c register_allocator.cc
emitter.o: emitter.h emitter.cc
	g++ -c emitter.cc
truc.o: token.h keywordtoken.h truc.cc addoptoken.h idtoken.h eoftoken.h muloptoken.h numtoken.h punctoken.h reloptoken.h buffer.h scanner.h parser.h symbol_table.h register.h register_allocator.h emitter.h
	g++ -c truc.cc keywordtoken.cc token.cc addoptoken.cc idtoken.cc eoftoken.cc muloptoken.cc numtoken.h punctoken.h reloptoken.h buffer.h scanner.h parser.h symbol_table.h register.h register_allocator.h emitter.h
truc: token.o keywordtoken.o truc.o addoptoken.o idtoken.o eoftoken.o muloptoken.o numtoken.o punctoken.o reloptoken.o buffer.o scanner.o parser.o symbol_table.o register.o register_allocator.o emitter.o
	g++ -o truc truc.o keywordtoken.o token.o addoptoken.o idtoken.o eoftoken.o muloptoken.o numtoken.o punctoken.o reloptoken.o buffer.o scanner.o parser.o symbol_table.o register.o register_allocator.o emitter.o
