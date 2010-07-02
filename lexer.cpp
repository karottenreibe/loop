
enum TokenType {
    tok_eof = -1,
    tok_loop = -2,
    tok_do = -3,
    tok_end = -4,
    tok_number = -5,
    tok_plus = -6,
    tok_minus = -7,
    tok_par_open = -8,
    tok_par_closed = -9,
    tok_assign = -10,
    tok_ident = -11,
    tok_sep = -12,
    tok_invalid = -99,
};

struct Token {
    int type;
    int ibuffer;
    std::string cbuffer;
    Token() {
        this->type = 0;
        this->ibuffer = 0;
        this->cbuffer = "";
    }
};

struct Lexer {
    char last_char;

    Lexer() {
        this->last_char = ' ';
    }

    Token next_token() {
        Token token;
        while (true) {
            // identifier := [a-z][a-z0-9]*
            if (isalpha(last_char) || (token.type == tok_ident && isdigit(last_char))) {
                token.type = tok_ident;
                token.cbuffer += last_char;
            // number := [0-9]+
            } else if (isdigit(last_char)) {
                token.type = tok_number;
                token.ibuffer = (token.ibuffer * 10) + last_char - 48;
            // (
            } else if (last_char == '(') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_par_open;
                last_char = getchar();
                break;
            // )
            } else if (last_char == ')') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_par_closed;
                last_char = getchar();
                break;
            // +
            } else if (last_char == '+') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_plus;
                last_char = getchar();
                break;
            // -
            } else if (last_char == '-') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_minus;
                last_char = getchar();
                break;
            // =
            } else if (last_char == '=') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_assign;
                last_char = getchar();
                break;
            // ;
            } else if (last_char == ';') {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_sep;
                last_char = getchar();
                break;
            // space
            } else if (isspace(last_char)) {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
            // EOF
            } else if (last_char == EOF) {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_eof;
                break;
            // invalid
            } else {
                // if something else has been read, we abort.
                // otherwise we ignore
                if (token.type != 0) {
                    break;
                }
                token.type = tok_invalid;
                token.cbuffer += last_char;
                last_char = getchar();
                break;
            }
            last_char = getchar();
        }
        // fix keywords
        if (token.type == tok_ident) {
            if (token.cbuffer == "loop") {
                token.type = tok_loop;
            } else if (token.cbuffer == "do") {
                token.type = tok_do;
            } else if (token.cbuffer == "end") {
                token.type = tok_end;
            }
        }
        return token;
    }
};



