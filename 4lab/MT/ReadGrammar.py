from enum import Enum
import sys

class State(Enum):
    START = 1
    HEADER = 2
    TOKENS = 3
    START_STATE = 4
    STATES = 5
    CODE = 6


def err():
    print("[$%#!$^#%^@$$!]")


inh = []
sint = []
header = []
codess = []
cur_rules = []
all_rules = []
terms = set()
tokens = []
start_no_term = ""
file_name = ""


def parse_token(s: str):
    name, reg = s.split(':', 1)
    name: str
    reg: str
    skip = False
    if name.startswith("?"):
        name = name.lstrip("?")
        skip = True
    terms.add(name)
    tokens.append("ans.push_back({ {\"" + name + "\", \"" +
                  reg.replace("\\", "\\\\") + "\"}, " + str(skip).lower() + " });")


def parse_state(s):
    name, inher, sintez = s.split("%")
    inh.append([name, inher])
    sint.append([name, sintez])
    return name


def parse_rule(s, name):
    names = []
    codes = []
    paramss = []
    ar = s.split("#")
    ind = 0
    for i in ar:
        if ind == 0:
            ind += 1
            continue  # init {}
        elif ind % 2 == 0:
            codes.append(i)
        else:
            n, params = i.split(".", 1)
            names.append(n)
            paramss.append(params)
        ind += 1

    ans1 = "{ {\"" + name + "\", false}, " + \
           "{" + ", ".join(
        map(lambda nm: "{\"" + nm + "\", " + ("true" if nm in terms else "false") + "}", names)) + "}" + \
           ", {" + ", ".join(map(lambda cod: "\"" + cod + "\"", codes)) + "}" + \
           ", {" + ", ".join(map(lambda par: "\"" + par + "\"", paramss)) + "} }"
    cur_rules.append(ans1.replace("\\", "\\\\"))


def get_attrs():
    ans = "std::unordered_map<string, vector<string>> init_inh() {\treturn {\n\t" +\
          ",\n\t".join(map(lambda staff: "{ \"" + staff[0] + "\", {\"" + staff[1] + "\"}}", inh)) +\
          "\n\t};\n}std::unordered_map<string, vector<string>> init_sint() {\treturn {\n\t" +\
          ",\n\t".join(map(lambda staff: "{ \"" + staff[0] + "\", {\"" + staff[1] + "\"}}", sint)) + "\n\t};\n}"
    return ans


def get_header():
    ans = "#n".join(header).replace("\\", "\\\\").replace("#", "\\") + "\";"
    return "return \"" + ans


def get_codes():
    ans = "#n".join(codess).replace("\\", "\\\\").replace("#", "\\").replace("\"", "\\\"") + "\";"
    return "return \"" + ans


def gen1(name):
    f = open(name, "w")
    print("//// gen1 start", file=f)
    print("string get_start_no_term() {\n\treturn \"" + start_no_term + "\";\n}", file=f)
    print("string get_header() {\n\t" + get_header() + "\n}", file=f)
    print("string get_codes() {\n\t" + get_codes() + "\n}", file=f)
    print("vector<std::pair<std::pair<string, string>, bool>> init_tokens() {"
          "\n\tvector<std::pair<std::pair<string, string>, bool>> ans = "
          "vector<std::pair<std::pair<string, string>, bool>>();\n\t" + "\n\t".join(tokens) +
          "\n\treturn ans;\n}", file=f)
    print(get_attrs(), file=f)
    print("//// gen1 fin\n\n", file=f)


def gen2(name):
    f = open(name, "w")
    print("Grammar g = Grammar(init_inh(), init_sint(), init_tokens(), {", file=f)
    for i in all_rules:
        print(i, ",", sep='', file=f)
    print("}, get_start_no_term());\n\tg.run(\"" + file_name + "/\");", file=f)


def main(grammar_name):
    delimit = "%%"

    with open(grammar_name) as inp:
        inp = inp.readlines()
        state = State.START
        cur_name = ""
        for line in inp:
            line = line.strip()
            if line.startswith("//") or line == "":
                continue
            if state == State.START:
                if line.startswith("%%"):
                    global file_name
                    file_name = line.lstrip("%")
                    state = State.HEADER
                else:
                    err()
                    return
            elif state == State.HEADER:
                if line == delimit:
                    state = State.TOKENS
                else:
                    header.append(line)
            elif state == State.TOKENS:
                if line == delimit:
                    state = state.START_STATE
                else:
                    parse_token(line)
            elif state == State.START_STATE:
                if line.startswith("%%"):
                    global start_no_term
                    start_no_term = line.lstrip("%")
                    state = State.STATES
                else:
                    err()
                    return
            elif state == State.STATES:
                if line == delimit:
                    state = state.CODE
                elif line.startswith(":"):
                    parse_rule(line.lstrip(":"), cur_name)
                elif line.startswith("|"):
                    parse_rule(line.lstrip("|"), cur_name)
                else:
                    global cur_rules
                    if len(cur_rules) != 0:
                        all_rules.append(",\n\t".join(cur_rules))
                        cur_rules = []
                    cur_name = parse_state(line)
            elif state == state.CODE:
                codess.append(line)

    if len(cur_rules) != 0:
        all_rules.append(",\n\t".join(cur_rules))
        cur_rules = []

    gen1("MT/gen1.txt")
    gen2("MT/gen2.txt")


if __name__ == "__main__":
    if (len(sys.argv) <= 1):
        print("Usage: <grammar_name.txt>")
        sys.exit(1)
    else:
        main(sys.argv[1])
