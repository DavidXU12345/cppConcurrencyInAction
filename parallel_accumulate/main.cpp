#include <vector>
#include <iostream>
#include "parrallel_accumulate.hpp"

int main()
{
    std::vector<int> v(1000000,1);
    int result=parallel_accumulate(v.begin(),v.end(),0);
    std::cout<<"Result: "<<result<<std::endl;
    return 0;
}