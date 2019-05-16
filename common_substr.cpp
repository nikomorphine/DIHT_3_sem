#include <iostream>
#include <limits>
#include <string>
#include <vector>

const int ALPHABET_SIZE = 256;
const int INF = std::numeric_limits<int>::max();

void buildClasses(const std::string& text, std::vector<long long>& suffArray,
                  std::vector<long long>& cnt, std::vector<long long>& classes) {
    for (long long i = 0; i < static_cast<long long>(text.size()) ; ++i)
        ++cnt[static_cast<int>(text[i])];
    for (int i = 1; i < ALPHABET_SIZE; ++i)
        cnt[i] += cnt[i - 1];
    for (long long i = 0; i < static_cast<long long>(text.size()); ++i)
        suffArray[--cnt[text[i]]] = i;
    classes[suffArray[0]] = 0;
    int classConst = 1;
    for (long long i = 1; i < static_cast<long long>(text.size()); ++i) {
        if (text[suffArray[i]] != text[suffArray[i - 1]])
            ++classConst;
        classes[suffArray[i]] = classConst - 1;
    }
}

void buildArray(const std::string& text, std::vector<long long>& suffArray,
                std::vector<long long>& cnt, std::vector<long long>& classes) {
    std::vector<long long> permutArr((int) text.size());
    std::vector<long long> classArr((int) text.size());
    int classConst = 1;
    for (long long power = 0; (1 << power) < static_cast<long long>(text.size()); ++power) {
        for (long long i = 0; i < static_cast<long long>(text.size()); ++i) {
            permutArr[i] = suffArray[i] - (1 << power);
            if (permutArr[i] < 0){
                permutArr[i] += static_cast<long long>(text.size());
            }
        }
        for (long long i = 0; i < static_cast<long long>(cnt.size()); ++i) {
            cnt[i] = 0;
        }
        for (long long i = 0; i < static_cast<long long>(text.size()); ++i)
            ++cnt[classes[permutArr[i]]];
        for (long long i = 1; i < classConst; ++i)
            cnt[i] += cnt[i - 1];
        for (long long i = static_cast<long long>(text.size()) - 1; i >= 0; --i)
            suffArray[--cnt[classes[permutArr[i]]]] = permutArr[i];
        classArr[suffArray[0]] = 0;
        for (long long i = 1; i < (int) text.size(); ++i) {
            const long long mid1 = (suffArray[i] + (1 << power)) % static_cast<long long>(text.size());
            const long long mid2 = (suffArray[i - 1] + (1 << power)) % static_cast<long long>(text.size());
            if (classes[suffArray[i]] != classes[suffArray[i-1]] || classes[mid1] != classes[mid2])
                ++classConst;
            classArr[suffArray[i]] = classConst - 1;
        }
        for (long long i = 0; i < static_cast<long long>(classes.size()); ++i) {
            classes[i] = classArr[i];
        }
    }
}

void buildSuffArray(const std::string& text, std::vector<long long>& suffArray) {
    std::vector<long long> classes(std::max(static_cast<int>(text.size()),
                                            (int) ALPHABET_SIZE) * sizeof(int));
    std::vector<long long> cnt(std::max(static_cast<int>(text.size()),
                                        (int) ALPHABET_SIZE) * sizeof(int));
    buildClasses(text, suffArray, cnt, classes);
    buildArray(text, suffArray, cnt, classes);
}

struct Node{
    Node() : parent(nullptr), depth(0), left(0), right(0), flag1(false), flag2(false) {}
    Node(Node* parent_, long long depth_) : parent(parent_), depth(depth_), left(0), right(0),
    flag1(false), flag2(false) {}
    
    Node* parent; //parent node
    std::vector<Node*> children; //vector of all children nodes
    //cnt_ is a counter for nodes
    //left right and maxDepth are characteristics of a part of string in the node
    long long depth;
    long long left;
    long long right;
    long long maxDepth;
    long long cnt_;
    //flag1 we can reach $, flag2 we can reach $ without #
    bool flag1, flag2;
};

class SuffixTree {
public:
    SuffixTree () : tree(nullptr) {}
    
    SuffixTree (const std::vector<long long> suffArray_, const std::vector<long long>& lcp_, const long long& posSymbol_) :
                suffArray(suffArray_), lcp(lcp_), posSymbol(static_cast<int>(posSymbol_)) {
                    calculatePositions(tree, tree, posSymbol);
                }
    std::vector<std::pair<long long, long long>> findKthCommonSubstring(Node* current, long long& k,
                                long long& ans, const long long& posSymbol) {
        std::vector<std::pair<long long, long long>> answer;
        if (ans != -1) {
            return answer;
        }
        if (k <= std::min(current->right, posSymbol) - current->left) {
            answer[answer.size() - 1].second = answer[answer.size() - 1].first + k;
            for (auto i : answer) {
                for (long long j = i.first; j < i.second; j++) {
                    std::cout << text[j];
                }
            }
            std::cout << std::endl;
            ans = INF;
            return answer;
        }
        
        k -= std::min(current->right, posSymbol) - current->left;
        
        for (auto child : current->children) {
            if (child->flag1 && child->flag2) {
                answer.push_back({child->left, child->right});
                findKthCommonSubstring(child, k, ans, posSymbol);
                if (ans != -1 && current->depth == 0) {
                    return answer;
                }
                answer.pop_back();
            }
        }
        return answer;
    }
    Node* tree;
private:
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
    
    Node* buildSuffixTree() {
        Node* root = new Node();
        Node* previous = root;
        for (long long i = 0; i < static_cast<int>(text.size()); ++i) {
            previous = addNextSuffix(previous, static_cast<int>(text.size()) - suffArray[i], lcp[i]);
        }
        return root;
    }
    
    void calculatePositions(Node* parent, Node* current, const long long& posSymbol) {
        current->maxDepth = current->depth;
        if (current->children.empty() && current->depth > 0) {
            current->left = static_cast<int>(text.size()) - current->maxDepth + parent->depth;
            current->right = current->left + current->depth - parent->depth;
            if (current->left > posSymbol) {
                current->flag2 = true;
            } else {
                current->flag1 = true;
            }
            return;
        }
        for (auto child : current->children) {
            calculatePositions(current, child, posSymbol);
            current->maxDepth = std::max(current->maxDepth, child->maxDepth);
            current->flag1 = std::max(current->flag1, child->flag1);
            if (child->left > posSymbol) {
                current->flag2 = current->flag2 and child->flag2;
            } else {
                current->flag1 = true;
            }
        }
        if (current->depth > 0) {
            current->left = static_cast<int>(text.size()) - current->maxDepth + parent->depth;
            current->right = current->left + current->depth - parent->depth;
        }
        if (current->left <= posSymbol) {
            current->flag1 = true;
        }
    }

    std::string text;
    std::vector<long long> suffArray;
    const std::vector<long long> lcp;
    int posSymbol;
};
//lcp -- laongest common prefix
void buildLCP(const std::string& text, const std::vector<long long>& suffArray,
              std::vector<long long>& lcp) {
    std::vector<long long> pos(static_cast<long long>(text.size()));
    for (long long i = 0; i < static_cast<long long>(text.size()) - 1; ++i) {
        pos[suffArray[i]] = i;
    }
    long long k = 0;
    for (long long i = 0; i < static_cast<long long>(text.size()) - 1; ++i) {
        if (k > 0) {
            k--;
        }
        if (pos[i] == static_cast<long long>(text.size()) - 1) {
            lcp[static_cast<long long>(text.size()) - 1] = -1;
            k = 0;
        } else {
            long long j = suffArray[pos[i] + 1];
            while (std::max(i + k, j + k) < static_cast<long long>(text.size()) &&
                   text[i + k] == text[j + k]) {
                k++;
            }
            lcp[pos[i]] = k;
        }
    }
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
    buildSuffArray(text, suffArray);
    buildLCP(text, suffArray, lcp);
    for (long long i = lcp.size() - 1; i > 1; --i) {
        lcp[i] = lcp[i - 1];
    }
    SuffixTree tree (suffArray, lcp, posSymbol);
    std::vector<std::pair<long long, long long>> answer =
    tree.findKthCommonSubstring(tree.tree, k, ans, posSymbol);
    if (ans == -1) {
        std::cout << ans << std::endl;
    }
    //if not we have already output text in findKthCommonSubstr
    return 0;
}
