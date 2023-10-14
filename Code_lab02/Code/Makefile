# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.c文件
CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CC) -o parser $(filter-out $(LFO),$(OBJS)) -lfl -ly

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v $(YFILE)

-include $(patsubst %.o, %.d, $(OBJS))

parser_:
	flex -o ./lex.yy.c ./lexical.l
	bison -o ./syntax.tab.c -d -v ./syntax.y
	gcc -g -c ./syntax.tab.c -o ./syntax.tab.o
	gcc -std=c99 -g -c -o semantic.o semantic.c
	gcc -std=c99 -g -c -o Tree.o Tree.c
	gcc -std=c99 -g -c -o main.o main.c
	gcc -g -o parser ./semantic.o ./Tree.o ./syntax.tab.o ./main.o -lfl -ly

# 定义的一些伪目标
.PHONY: clean test
test:
	./parser ../Test/test1.cmm
	./parser ../Test/test2.cmm
	./parser ../Test/test3.cmm
	./parser ../Test/test4.cmm
	./parser ../Test/test5.cmm
	./parser ../Test/test6.cmm
	./parser ../Test/test7.cmm
	./parser ../Test/test8.cmm
	./parser ../Test/test9.cmm
	./parser ../Test/test10.cmm
	./parser ../Test/test11.cmm
	./parser ../Test/test12.cmm
	./parser ../Test/test13.cmm
	./parser ../Test/test14.cmm
	./parser ../Test/test15.cmm
	./parser ../Test/test16.cmm
	./parser ../Test/test17.cmm
	./parser ../Test/test_o1.cmm
	./parser ../Test/test_o2.cmm
	./parser ../Test/test_o3.cmm
	./parser ../Test/test_o4.cmm
	./parser ../Test/test_o5.cmm
	./parser ../Test/test_o6.cmm
clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
