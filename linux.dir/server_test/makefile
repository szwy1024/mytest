src1=$(wildcard ../mylib/src/*.c)
obj1=$(patsubst ../mylib/src/%.c,../mylib/src/%.o,$(src1))
lib1=$(patsubst ../mylib/src/%.o,../mylib/lib/lib%.so,$(obj1))

src2=$(wildcard ./*.c)
obj2=$(patsubst ./%.c,./%,$(src2))

myargs=-g -Wall
lib_path=-L../mylib/lib
inc_path=-I../mylib/inc
lib_name=-lwrap

all:$(obj2)

$(obj2):./%:./%.c $(lib1)
	gcc $< -o $@ $(myargs) $(lib_name) $(lib_path) $(inc_path)
	export LD_LIBRARY_PATH=../mylib/lib

$(lib1):$(obj1)
	gcc -shared -o $@  $^ -fPIC

$(obj1):$(src1)
	gcc -c $< -o $@ $(myargs) $(inc_path) -fPIC

clean:
	-rm -rf  $(obj1) $(lib1) $(obj2)

.PHONY:clean all
