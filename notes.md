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

### OpenMP Usage with XCode

I was able to compile code using OpenMP (OMP) with XCode, however debugging does not work as well. In order to enable XCode for OMP, one first needs to get an OMP-enabled compiler, such as: GCC.

1. First get [Homebrew](https://brew.sh/).
2. Then it is important to get GCC, possibly with:
```
brew install gcc --without-multilib
```.
3. Then locate the GCC executable, which should be something like:
```
usr/local/bin/gcc-N
```
The `N` is the version and Homebrew appends it.
4. Later, create a new XCode Build Rule for the desired project. Under Build Rules, press the addition button and select *C Source Files* for **Process** and *Custom Script* for **Using:**.
5. After, enter the script to use, something like:
`/usr/local/bin/gcc-N -g -fopenmp -Wall -o ${CODESIGNING_FOLDER_PATH} Project-Name/*.c`
6. Lastly, make sure something like `$(DERIVED_FILE_DIR)/myfile` appears under Output Files, because without it, the script never gets called. (Honestly I do not know what is the use of this other than for making the script work.)
7. Et Voilà.

##Out of Class's Scope Notes
##Compilers
* A tool for building own compilers: [Lex Yacc](http://dinosaur.compilertools.net/).

