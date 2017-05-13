CFLAGS=-Wall -O3 -std=c99 -I.
LFLAGS=
LIBJSON_OBJ=\
 json.o json_arr.o\
 json_false.o json_true.o\
 json_null.o json_num.o\
 json_str.o json_obj.o
LIBJSON_DIR=libjson
DEPS=./libjson/json.h

libjson: $(patsubst %,$(LIBJSON_DIR)/%,$(LIBJSON_OBJ)) err/err.o
	ar rcs libjson.a $^ 
$(LIBJSON_DIR)/%.o: %.c
	$(CC) -o $@ $< -c $(CFLAGS)

err/err.o: err/err.c err/err.h
	$(CC) -o $@ $< -c $(CFLAGS)
	
clean:
	rm -f $(LIBJSON_DIR)/*.o  err/*.o *.a