#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class AhoCorasicMachine {
public:
    AhoCorasicMachine(): num_of_strings(0), current_state(0) {
        // emplace root (root hasn't a parent).
        vertices.emplace_back(Vertex(-1, 0));
    }
    // add new string to trie.
    void add_template(const std::string& new_template) {
        // add transitions.
        int current_vertex = 0;
        for(char current_character: new_template) {
            // check if we already had this transition or not.
            if(vertices[current_vertex].direct_transitions.find(current_character) == vertices[current_vertex].direct_transitions.end()) {
                // create a new vertex.
                vertices.emplace_back(Vertex(current_vertex, current_character));
                vertices[current_vertex].direct_transitions[current_character] = static_cast<int>(vertices.size() - 1);
            }
            current_vertex = vertices[current_vertex].direct_transitions[current_character];
        }
        // add terminal vertex.
        vertices[current_vertex].string_indexes.push_back(num_of_strings++);
    }
    // step of algorithm.
    std::vector<int>& step(char character) {
        int current_vertex = current_state = get_transition(current_state, character);
        if(!vertices[current_vertex].result_compress.empty())
            return vertices[current_vertex].result_compress;
        // while current_vertex isn't root.
        while(current_vertex) {
            for(auto i: vertices[current_vertex].string_indexes) {
                vertices[current_state].result_compress.push_back(i);
            }
            current_vertex = get_compressed_suffix_link(current_vertex);
        }
        return vertices[current_state].result_compress;
    }
private:
    struct Vertex {
        Vertex(int parent, char from_parent_character): parent(parent), from_parent_character(from_parent_character) {}
        int parent;
        char from_parent_character;
        int suffix_link = -1;
        int compressed_suffix_link = -1;
        std::vector<int> result_compress;
        std::vector<int> string_indexes;
        std::unordered_map<char, int> direct_transitions;
        std::unordered_map<char, int> transitions;
    };
    // get new vertex by vertex and character.
    int get_transition(int vertex, char character) {
        // if we haven't saved value for transition.
        if(vertices[vertex].transitions.find(character) == vertices[vertex].transitions.end()) {
            // try to find direct transit in trie.
            auto it_direct_transition = vertices[vertex].direct_transitions.find(character);
            if(it_direct_transition != vertices[vertex].direct_transitions.end()) {
                vertices[vertex].transitions[character] = it_direct_transition->second;
            } else if(vertex == 0) {
                // if we in root we can transit only in root (if we haven't direct transit in trie).
                vertices[vertex].transitions[character] = 0;
            } else {
                // try to calculate new compressed suffix link and save it.
                vertices[vertex].transitions[character] = get_transition(get_suffix_link(vertex), character);
 
            }
        }
        // return saved value.
        return vertices[vertex].transitions[character];
    }
    // get suffix link.
    int get_suffix_link(int vertex) {
        if(vertices[vertex].suffix_link == -1) {
            if(vertex == 0 || vertices[vertex].parent == 0) {
                vertices[vertex].suffix_link = 0;
            } else {
                vertices[vertex].suffix_link = get_transition(get_suffix_link(vertices[vertex].parent), vertices[vertex].from_parent_character);
            }
        }
        return vertices[vertex].suffix_link;
    }
    int get_compressed_suffix_link(int vertex) {
        if(vertices[vertex].compressed_suffix_link == -1) {
            // if vertex is terminal.
            if(!vertices[get_suffix_link(vertex)].string_indexes.empty()) {
                vertices[vertex].compressed_suffix_link = get_suffix_link(vertex);
            } else if(get_suffix_link(vertex) == 0) {
                vertices[vertex].compressed_suffix_link = 0;
            } else {
                vertices[vertex].compressed_suffix_link = get_compressed_suffix_link(get_suffix_link(vertex));
            }
        }
        return vertices[vertex].compressed_suffix_link;
    }
    // current vertex (state of machine).
    int current_state;
    // store of vertices.
    std::vector<Vertex> vertices;
    // num of strings saved in trie.
    int num_of_strings;
};
 
// return num of strings in template
int find_matches(const std::string& template_string, const std::string& processed_string, std::vector<int>& matches) {
    std::vector<int> string_offsets;
    AhoCorasicMachine aho_corasic_machine;
    std::string current_string;
    // find templates.
    for(int i = 0; i < template_string.size(); ++i) {
        if(template_string[i] != '?') {
            current_string += template_string[i];
        } else if(!current_string.empty()) {
            aho_corasic_machine.add_template(current_string);
            // write offsets for every string.
            string_offsets.push_back(i - 1);
            current_string = "";
        }
    }
    // process the case with template in the end.
    if(!current_string.empty()) {
        aho_corasic_machine.add_template(current_string);
        // write offsets for every string.
        string_offsets.push_back(template_string.size() - 1);
        current_string = "";
    }
    matches.resize(processed_string.size(), 0);
    // run aho corasic character by character on processed_string.
    for(int i = 0; i < processed_string.size(); ++i) {
        std::vector<int> indexes_list = aho_corasic_machine.step(processed_string[i]);
        for(auto string_index: indexes_list) {
            if(i >= string_offsets[string_index]) {
                matches[i - string_offsets[string_index]] += 1;
            }
        }
    }
    return static_cast<int>(string_offsets.size());
}
 
int main() {
    std::string template_string;
    std::string processed_string;
    std::vector<int> matches;
    std::cin >> template_string;
    std::cin >> processed_string;
    int num_of_strings = find_matches(template_string, processed_string, matches);
    // print result of matching.
    if(template_string.size() <= processed_string.size()) {
        for(int i = 0; i < (matches.size() - template_string.size() + 1); ++i) {
            if(matches[i] == num_of_strings) {
                std::cout << i << " ";
            }
        }
    }
}

