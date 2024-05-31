# mystd
My educational implementation of C++ STL containers.
This library uses concepts that are added in the С++20.

#Now implemented
- forward list with support for iterators and custom allocators

##How to use
If you are using cmake, add these commands to your CMakeLists.txt:
```
target_link_directories(your_target PRIVATE ${PATH_TO_THE_LIB}/mystd)

target_link_libraries(your_target mystd)
```
After that, you will be able to include the library headers:
```
#include "forward_list.hpp"
```
The library uses the namespace `mystd` and CamelCase in container names. 
The names of the methods correspond to the interface of the STL containers.

##Simple sample:
```
#include <iostream>
#include <algorithm>

#include "forward_list.hpp"

int main() {
    mystd::ForwardList<int> list(10);
    std::iota(list.begin(), list.end(), 1);
    list.push_front(100);
    list.pop_front();

    for(int num : list) {
        std::cout << num << std::endl;
    }

    return 0;
}
```



