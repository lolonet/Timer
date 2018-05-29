
# 将 .o 文件集中到 ./obj
$(shell mkdir -p ./obj)

objFile = main.o time.o
obj = ./obj
bin = timer

all: $(objFile)
	@cd $(obj); $(CXX) -Wall -o ../$(bin) $(objFile)
	@echo "make succ"

main.o: 
	$(CXX) -Wall -c  main.cpp -o $(obj)/main.o

time.o:
	$(CXX) -Wall -c Timer.cpp -o $(obj)/time.o

.PHONY : clean

clean:
	-rm -rf $(bin) $(obj)


