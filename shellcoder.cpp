#include <iostream>

void helloWorld(){
    std::cout << "Hello World !" << std::endl;
}

int main(void){
    std::cout << "Address of helloworld() function: " << (void*)helloWorld << std::endl;

    const int t = sizeof(&helloWorld);
    unsigned char* code = (unsigned char*)&helloWorld;
    const int numBytes = sizeof(*code);
    std::cout << "Function has " << numBytes << " bytes: " << std::endl;
    for(int i = 0; i < numBytes; i++){
        printf("%02x", code[i]);
    }
}