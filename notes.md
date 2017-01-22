#Class Notes
##20170119
### Pointers
1. Arrays are pointers that are not mutable:

```c++
int numbers[10];
int * p;
p = numbers;
p++;
numbers = p; //This is not possible because numbers is an array, i.e. an immutable pointer.
```

##Out of Class's Scope Notes
##Compilers
* A tool for building own compilers: [Lex Yacc](http://dinosaur.compilertools.net/).
* 