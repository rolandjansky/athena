#include <iostream>
#include <vector>
#include <functional>
#include <string>
/*First test file . Check whether c++1x compiles wwll
 S.I
 */


int main(){
    
    std::vector<std::function<void(std::string)>> functions;
    functions.push_back([](std::string say){std::cout<<say<<std::endl;});
    auto k=functions[0];
    k("AAA");
    k("B");
    k("CCCCCC");
    return 0;
    
}
