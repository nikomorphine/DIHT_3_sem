#include <iostream>
#include <string>
#include <vector>

std::vector<int> zFunction(const std::string& someText) {
    //array for zFunction of template
    std::vector<int> zFunc(someText.length());
    //evaluating zFunction of temolate
    int left = 0, right = 0;
    for (int i = 1; i < someText.length(); ++i) {
        //initialisation of zFunction element
        zFunc[i] = std::max(0, std::min(zFunc[i - left], right - i));
        //incrementing zFunction value if index is more than right
        while (i + zFunc[i] < someText.length() && someText[zFunc[i]] == someText[i + zFunc[i]]) {
            zFunc[i]++;
        }
        //redefinition of left and right indexes according to zBlock in which we are in
        if (i + zFunc[i] > right) {
            left = i;
            right = left + zFunc[i] - 1;
        }
    }
    return zFunc;
}
//function to calculate all indexes, where the template was found
void substringEntrance(std::string temp, std::string text) {
    std::vector<int> zFuncTemp = zFunction(temp);
    int left = -1, right = 0, counter = 0; //initialising variable counter to keep track of zFunction value without storing zFunction itself
    for (int i = 0; i < text.length(); ++i) {
        //presetting counter as we do in the zFunction algorithm
        counter = std::max(0, std::min(zFuncTemp[i - left], right - i));
        //incrementing the counter if index is greater then right position
        while (i + counter < text.length() && counter < temp.length() && text[i + counter] == temp[counter]) {
            counter++;
        }
        //redefinition of left and right indexes according to zBlock in which we are in
        if (i + counter > right) {
            left = i;
            right = left + counter;
        }
        //other case is not bothering us because in this case we can't get value equal to template length
        if ((counter) == temp.length()) {
            std::cout << i << ' ';
        }
    }
}

int main() {
    std::string str, temp;
    std::cin >> temp >> str;

    substringEntrance(temp, str);
}
