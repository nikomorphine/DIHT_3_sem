#include <iostream>
#include <limits>
#include <string>
#include <vector>

const int ALPHABET = 256;
const int INF = std::numeric_limits<int>::max();

std::vector<long long> suffArr(const std::string& text, const long long& textSize,
                               std::vector<long long>& suffArray) {
    std::vector<long long> classes(std::max((int)textSize, (int) ALPHABET) * sizeof(int));
    std::vector<long long> cnt(std::max((int)textSize, (int) ALPHABET) * sizeof(int));
    for (long long i = 0; i < textSize; ++i)
        ++cnt[(int) (text[i])];
    for (int i = 1; i < ALPHABET; ++i)
        cnt[i] += cnt[i - 1];
    for (long long i = 0; i < textSize; ++i)
        suffArray[--cnt[text[i]]] = i;
    classes[suffArray[0]] = 0;
    int classConst = 1;
    for (long long i = 1; i < textSize; ++i) {
        if (text[suffArray[i]] != text[suffArray[i - 1]])
            ++classConst;
        classes[suffArray[i]] = classConst - 1;
    }
    std::vector<long long> pn(textSize);
    std::vector<long long> cn(std::max((int)textSize, (int) ALPHABET) * sizeof(int));
    for (long long h = 0; (1 << h) < textSize; ++h) {
        for (long long i = 0; i < textSize; ++i) {
            pn[i] = suffArray[i] - (1 << h);
            if (pn[i] < 0){
                pn[i] += textSize;
            }
        }
        for (long long i = 0; i < cnt.size(); ++i) {
            cnt[i] = 0;
        }
        for (long long i = 0; i < textSize; ++i)
            ++cnt[classes[pn[i]]];
        for (int i = 1; i < classConst; ++i)
            cnt[i] += cnt[i - 1];
        for (long long i = textSize - 1; i >= 0; --i)
            suffArray[--cnt[classes[pn[i]]]] = pn[i];
        cn[suffArray[0]] = 0;
        classConst = 1;
        for (long long i = 1; i < textSize; ++i) {
            long long mid1 = (suffArray[i] + (1 << h)) % textSize;
            long long mid2 = (suffArray[i - 1] + (1 << h)) % textSize;
            if (classes[suffArray[i]] != classes[suffArray[i-1]] || classes[mid1] != classes[mid2])
                ++classConst;
            cn[suffArray[i]] = classConst - 1;
        }
        for (long long i = 0; i < classes.size(); ++i) {
            classes[i] = cn[i];
        }
    }
    return suffArray;
}

struct Node{
    Node() {
        parent = nullptr;
        depth = 0;
        left = 0;
        right = 0;
        flag1 = false;
        flag2 = false;
    }
    Node(Node* parent_, long long depth_){
        parent = parent_;
        this->depth = depth_;
        left = 0;
        right = 0;
        flag1 = false;
        flag2 = false;
    }
    Node* parent;
    std::vector<Node*> children;
    long long depth, left, right, maxDepth, cnt_;
    bool flag1, flag2;
};

Node* addNextSuffix(Node* previous, const long long& textSize, const long long& lcp) {
    if (previous->depth == 0 || previous->depth == lcp) {
        Node* added = new Node(previous, textSize);
        previous->children.push_back(added);
        return added;
    } else if (previous->parent->depth < lcp) {
        Node* inserted = new Node(previous->parent, lcp);
        previous->parent->children.pop_back();
        previous->parent->children.push_back(inserted);
        inserted->children.push_back(previous);
        previous->parent = inserted;
    }
    return addNextSuffix(previous->parent, textSize, lcp);
}

Node* buildSuffixTree(const std::vector<long long>& suffArray, const std::vector<long long>& lcp,
                      const long long& textSize) {
    Node* root = new Node();
    Node* previous = root;
    for (long long i = 0; i < textSize; ++i) {
        previous = addNextSuffix(previous, textSize - suffArray[i], lcp[i]);
    }
    return root;
}

void calculatePositions(Node* parent, Node* current, const long long& textSize, const long long& posSymbol) {
    current->maxDepth = current->depth;
    if (current->children.empty() && current->depth > 0) {
        current->left = textSize - current->maxDepth + parent->depth;
        current->right = current->left + current->depth - parent->depth;
        if (current->left > posSymbol) {
            current->flag2 = true;
        } else {
            current->flag1 = true;
        }
        return;
    }
    for (auto child : current->children) {
        calculatePositions(current, child, textSize, posSymbol);
        current->maxDepth = std::max(current->maxDepth, child->maxDepth);
        current->flag1 = std::max(current->flag1, child->flag1);
        if (child->left > posSymbol) {
            current->flag2 = std::max(current->flag2, child->flag2);
        } else {
            current->flag1 = true;
        }
    }
    if (current->depth > 0) {
        current->left = textSize - current->maxDepth + parent->depth;
        current->right = current->left + current->depth - parent->depth;
    }
    if (current->left <= posSymbol) {
        current->flag1 = true;
    }
}

void findKthCommonSubstring(Node* current, long long& k, const std::string& text, long long& ans,
                            std::vector<std::pair<long long, long long>>& answer, const long long& posSymbol) {
    if (ans != -1) {
        return;
    }
    //std::cout << current->right << current->left << std::endl;
    if (k <= std::min(current->right, posSymbol) - current->left) {
        answer[answer.size() - 1].second = answer[answer.size() - 1].first + k;
        for (auto i : answer) {
            for (long long j = i.first; j < i.second; j++) {
                std::cout << text[j];
            }
        }
        std::cout << std::endl;
        ans = INF;
        return;
    }
    
    k -= std::min(current->right, posSymbol) - current->left;
    
    for (auto child : current->children) {
        if (child->flag1 && child->flag2) {
            answer.push_back({child->left, child->right});
            findKthCommonSubstring(child, k, text, ans, answer, posSymbol);
            if (ans != -1 && current->depth == 0) {
                return;
            }
            answer.pop_back();
        }
    }
}

std::vector<long long> kasai(const std::string& text, const long long& textSize,
                             std::vector<long long>& suffArray, std::vector<long long>& lcp) {
    std::vector<long long> pos(textSize);
    for (long long i = 0; i < textSize - 1; ++i) {
        pos[suffArray[i]] = i;
    }
    long long k = 0;
    for (long long i = 0; i < textSize - 1; ++i) {
        if (k > 0) {
            k--;
        }
        if (pos[i] == textSize - 1) {
            lcp[textSize - 1] = -1;
            k = 0;
        } else {
            long long j = suffArray[pos[i] + 1];
            while (std::max(i + k, j + k) < textSize && text[i + k] == text[j + k]) {
                k++;
            }
            lcp[pos[i]] = k;
        }
    }
    return lcp;
}

int main() {
    std::string text1, text2;
    long long textSize;
    std::cin >> text1 >> text2;
    long long posSymbol = (long long) text1.length();
    std::string text = text1 + '$' + text2 + '#';
    long long k;
    long long ans = -1;
    std::cin >> k;
    textSize = (long long) text.length();
    std::vector<long long> suffArray(textSize);
    std::vector<long long> lcp(textSize);
    suffArr(text, textSize, suffArray);
    kasai(text, textSize, suffArray, lcp);
    for (long long i = lcp.size() - 1; i > 1; --i) {
        lcp[i] = lcp[i - 1];
    }
    Node* tree;
    tree = buildSuffixTree(suffArray, lcp, textSize);
    calculatePositions(tree, tree, textSize, posSymbol);
    std::vector<std::pair<long long, long long>> answer;
    findKthCommonSubstring(tree, k, text, ans, answer, posSymbol);
    if (ans == -1) {
        std::cout << ans << std::endl;
    }
    
    return 0;
}
