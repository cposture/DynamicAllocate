objects = DynamicAllocate.o
target = DynamicAllocate
target:$(objects)
	gcc -g $(objects) -o $(target) 

$(objects):%.o:%.cpp
objects:
	gcc -g -c %< -o $(objects) 
clean:
	rm -rf $(objects) $(target)
