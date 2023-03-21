OBJS = 
OBJS += fibo.o main.o

compile: $(OBJS)
	gcc $^ -o ../fibo

$(OBJS) : %.o : %.c
	gcc -c -I ../include $< -o $@

.PHONY: clean
clean:
	rm *.o