#include <iostream>
#include <vector>
#include <string>

const int alphabet = 256;

std::vector<int> suffArr(const std::string& text, int textSize, std::vector<int>& suffArray) {
    std::vector<int> classes(textSize);
    std::vector<int> cnt(alphabet * sizeof(int));
    for (int i = 0; i < textSize; ++i)
        ++cnt[(int) (text[i])];
    for (int i = 1; i < alphabet; ++i)
        cnt[i] += cnt[i - 1];
    for (int i = 0; i < textSize; ++i)
        suffArray[--cnt[text[i]]] = i;
    classes[suffArray[0]] = 0;
    int classConst = 1;
    for (int i = 1; i < textSize; ++i) {
        if (text[suffArray[i]] != text[suffArray[i - 1]])
            ++classConst;
        classes[suffArray[i]] = classConst - 1;
    }
    std::vector<int> pn(textSize);
    std::vector<int> cn(textSize);
    for (int h = 0; (1 << h) < textSize; ++h) {
        for (int i = 0; i < textSize; ++i) {
            pn[i] = suffArray[i] - (1 << h);
            if (pn[i] < 0){
                pn[i] += textSize;
            }
        }
        for (int i = 0; i < cnt.size(); ++i) {
            cnt[i] = 0;
        }
        for (int i = 0; i < textSize; ++i)
            ++cnt[classes[pn[i]]];
        for (int i=1; i < classConst; ++i)
            cnt[i] += cnt[i - 1];
        for (int i = textSize - 1; i >= 0; --i)
            suffArray[--cnt[classes[pn[i]]]] = pn[i];
        cn[suffArray[0]] = 0;
        classConst = 1;
        for (int i = 1; i < textSize; ++i) {
            int mid1 = (suffArray[i] + (1 << h)) % textSize;
            int mid2 = (suffArray[i - 1] + (1 << h)) % textSize;
            if (classes[suffArray[i]] != classes[suffArray[i-1]] || classes[mid1] != classes[mid2])
                ++classConst;
            cn[suffArray[i]] = classConst - 1;
        }
        for (int i = 0; i < classes.size(); ++i) {
            classes[i] = cn[i];
        }
    }
    return suffArray;
}

std::vector<int> kasai(const std::string& text, int textSize, std::vector<int>& suffArray, std::vector<int>& lcp) {
    std::vector<int> pos(textSize);
    for (int i = 0; i < textSize - 1; ++i) {
        pos[suffArray[i]] = i;
    }
    int k = 0;
    for (int i = 0; i < textSize - 1; ++i) {
        if (k > 0) {
            k--;
        }
        if (pos[i] == textSize - 1) {
            lcp[textSize - 1] = -1;
            k = 0;
        } else {
            int j = suffArray[pos[i] + 1];
            while (std::max(i + k, j + k) < textSize && text[i + k] == text[j + k]) {
                k++;
            }
            lcp[pos[i]] = k;
        }
    }
    return lcp;
}

int main() {
    std::string text;
    int textSize;
    std::cin >> text;
    text += '#';
    textSize = (int) text.length();
    std::vector<int> suffArray(textSize);
    std::vector<int> lcp(textSize);
    suffArr(text, textSize, suffArray);
    kasai(text, textSize, suffArray, lcp);
    lcp[textSize - 1] = 0;
    int countString = 0;
    for (int i = 0; i < textSize; ++i) {
        countString += (textSize - suffArray[i] - lcp[i]);
    }
    std::cout << countString - textSize;
    
    return 0;
}
