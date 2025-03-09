#include "assembler.h"

// Variáveis dos arquivos de entrada e saída
FILE* input;
FILE* output;

// Variáveis para o lexer e parser
Lexer *lexer;
Parser *parser;

// Constante para os mnemônicos
const Instruction instructions[] = {
    {"NOP", 0x00},
    {"STA", 0x10},
    {"LDA", 0x20},
    {"ADD", 0x30},
    {"OR",  0x40},
    {"AND", 0x50},
    {"NOT", 0x60},
    {"JMP", 0x80},
    {"JN",  0x90},
    {"JZ",  0xA0},
    {"HLT", 0xF0}
};

// Inicializa o lexer para processar o arquivo de entrada
Lexer* create_lexer(FILE* input) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->input_file = input;
    lexer->line[0] = '\0';
    lexer->line_ptr = lexer->line;
    lexer->line_number = 0;
    lexer->in_comment = 0;
    
    return lexer;
}

// Verifica se um caractere é um delimitador
int is_delimiter(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';');
}

// Verifica se a linha contém um comentário
int is_comment(const char* line) {
    while (*line && (*line == ' ' || *line == '\t')) {
        line++;
    }
    return (*line == ';');
}

// Lê o próximo token do lexer
char* read_token(Lexer* lexer) {
    char* token_start = lexer->line_ptr;
    
    while (*(lexer->line_ptr) && !is_delimiter(*(lexer->line_ptr))) {
        (lexer->line_ptr)++;
    }
    
    if (*(lexer->line_ptr)) {
        *(lexer->line_ptr) = '\0'; 
        (lexer->line_ptr)++; 
    }
    
    return token_start;
}

// Determina o tipo do token lido
TokenType determine_type(const char* token_start) {
    if (token_start[0] == '.') {
        return DIRECTIVE;
    } 
    else if (isdigit(token_start[0]) || 
            (token_start[0] == '0' && (token_start[1] == 'x' || token_start[1] == 'X'))) {
        return NUMBER;
    } 
    else if (find_instruction(instructions, sizeof(instructions) / sizeof(Instruction), token_start) != -1) {
        return MNEMONIC;
    }
    /*
     else if (strcmp(token_start, "DB") == 0){
        return VARIABLE_TYPE;
    } 
    else {
        if (isalpha(token_start[0])) {
            for (int i = 1; token_start[i] != '\0'; i++) {
                if (!isalnum(token_start[i])) {
                    return NONE; 
                }
            }
            return VARIABLE;
        }
    }
    */
   
    return NONE; 
}

// Obtém o próximo token do lexer
Token* next_token(Lexer* lexer) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) return NULL;
    
    token->type = NONE;
    token->value = NULL;
    token->line_number = lexer->line_number;

    if (*(lexer->line_ptr) == '\0') {
        if (fgets(lexer->line, MAX_LENGTH, lexer->input_file) == NULL) {
            token->type = TOKEN_EOF;
            return token;
        }
        
        lexer->line_ptr = lexer->line;
        lexer->line_number++;

        if (is_comment(lexer->line)) {
            free(token);
            return next_token(lexer);
        }
        
        token->line_number = lexer->line_number;
    }

    while (*(lexer->line_ptr) && is_delimiter(*(lexer->line_ptr))) {
        if (*(lexer->line_ptr) == ';') {
            *(lexer->line_ptr) = '\0';
            token->type = TOKEN_EOL;
            return token;
        }
        (lexer->line_ptr)++;
    }

    if (*(lexer->line_ptr) == '\0') {
        token->type = TOKEN_EOL;
        return token;
    }

    char* token_start = read_token(lexer);
    token->type = determine_type(token_start);
    token->value = strdup(token_start);
    
    return token;
}

// Libera a memória alocada para um token
void token_destroy(Token* token) {
    if (token) {
        if (token->value) {
            free(token->value);
        }
        free(token);
    }
}

// Converte um número em string para um valor inteiro
int parse_number(const char* str) {
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        return strtol(str + 2, NULL, 16); 
    }
    return atoi(str); 
}

// Cria e inicializa um parser associado a um lexer e um conjunto de instruções
Parser* create_parser(Lexer* lexer) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    memset(parser->memory, 0, MEMORY_SIZE * sizeof(unsigned short));
    parser->code_address = 0;
    parser->max_address = 0;
    parser->in_data_section = 0;
    parser->instructions = instructions;
    parser->num_instructions = sizeof(instructions) / sizeof(Instruction);
    
    return parser;
}

// Lê o arquivo de entrada e processa seus tokens
int read_file(Parser* parser) {
    Token* token;
    
    while ((token = next_token(parser->lexer))->type != TOKEN_EOF) {
        if (token->type == TOKEN_EOL) {
            token_destroy(token);
            continue;
        }
        
        if (token->type == DIRECTIVE) {
            if (strcmp(token->value, ".DATA") == 0) {
                parser->in_data_section = 1;
                token_destroy(token);
                continue;
            } else if (strcmp(token->value, ".CODE") == 0) {
                parser->in_data_section = 0;
                parser->code_address = 0;  
                token_destroy(token);
                continue;
            }
        }
        
        if (parser->in_data_section) {
            if (!read_data(parser, token)) {
                return 0;
            }
        } else {
            if (!read_code(parser, token)) {
                return 0;
            }
        }
    }
    
    token_destroy(token);
    return 1;
}

// Processa um token como uma variável e armazena na memória do parser
int read_data(Parser* parser, Token* token) {
    /*
    if (token->type != VARIABLE) {
        printf("Erro: Nome da variável esperada na linha %d\n", token->line_number);
        token_destroy(token);
        return 0;
    }
    */
    
    if (token->type != NUMBER) {
        printf("Erro: Nome da variável esperada na linha %d\n", token->line_number);
        token_destroy(token);
        return 0;
    }

    int address = parse_number(token->value);
    token_destroy(token);

     /*
    token = next_token(parser->lexer);
    if (token->type != VARIABLE_TYPE) {
        printf("Erro: Tipo do dado esperado depois do nome da variável na linha: %d\n", token->line_number);
        token_destroy(token);
        return 0;
    }
    */
    
    token = next_token(parser->lexer);
    if (token->type != NUMBER) {
        printf("Erro: Valor esperado depois do nome da variável na linha: %d\n", token->line_number);
        token_destroy(token);
        return 0;
    }
    
    int value = parse_number(token->value);
    token_destroy(token);
    
    parser->memory[address] = value;
    
    if (address > parser->max_address) {
        parser->max_address = address;
    }
    
    return 1;
}

// Processa um token como uma instrução e armazena na memória do parser
int read_code(Parser* parser, Token* token) {
    if (token->type != MNEMONIC) {
        printf("Erro: Mnemônico esperado na linha: %d\n", token->line_number);
        token_destroy(token);
        return 0;
    }
    
    int instr_index = find_instruction(parser->instructions, parser->num_instructions, token->value);
    if (instr_index == -1) {
        printf("Erro: Código mnemônico desconhecido %s, na linha %d\n", token->value, token->line_number);
        token_destroy(token);
        return 0;
    }
    
    parser->memory[parser->code_address] = parser->instructions[instr_index].opcode;
    
    if (strcmp(token->value, "NOP") != 0 && strcmp(token->value, "HLT") != 0) {
        token_destroy(token);
        
        token = next_token(parser->lexer);
        if (token->type != NUMBER) {
            printf("Erro: Número esperado para instrução na linha: %d\n", token->line_number);
            token_destroy(token);
            return 0;
        }
        
        int operand = parse_number(token->value);
        parser->code_address++;
        parser->memory[parser->code_address] = operand;
    }
    
    token_destroy(token);
    parser->code_address++;
    
    if (parser->code_address > parser->max_address) {
        parser->max_address = parser->code_address;
    }
    
    return 1;
}

// Busca um mnemônico na lista de instruções e retorna seu índice
int find_instruction(const Instruction* instructions, int num_instructions, const char* mnemonic) {
    for (int i = 0; i < num_instructions; i++) {
        if (strcmp(instructions[i].mnemonic, mnemonic) == 0) {
            return i;
        }
    }
    return -1; 
}

// Função principal que inicializa o lexer e parser, lê o arquivo e gera o bytecode
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Número de parâmetros diferente do esperado. Use ./<program> file.txt\n");
        return 0;
    }

    input = fopen(argv[1], "r");
    if (!input) {
        printf("Não foi possível abrir o arquivo .txt\n");
        return 0;
    }

    lexer = create_lexer(input);
    if (!lexer) {
        printf("Falha ao iniciar lexer\n");
        fclose(input);
        return 0;
    }

    parser = create_parser(lexer);
    if (!parser) {
        printf("Falha ao iniciar parser\n");
        free(lexer);
        fclose(input);
        return 0;
    }

    if (!read_file(parser)) {
        free(parser);
        free(lexer);
        fclose(input);
        return 0;
    }

    unsigned short temp_memory[MEMORY_SIZE] = {0};
    for (int i = 0; i <= parser->max_address; i++) {
        temp_memory[i] = parser->memory[i];
    }
    
    parser->memory[0] = 0x4e03;  
    parser->memory[1] = 0x5244;  
    
    for (int i = 0; i <= parser->max_address; i++) {
        parser->memory[i + 2] = temp_memory[i];
    }
    
    parser->max_address += 2;

    output = fopen("bytecode.mem", "wb");
    if (!output) {
        printf("Falha ao gerar arquivo binário\n");
        return 0;
    }

    fwrite(parser->memory, 2, parser->max_address + 1, output);
    fclose(output);

    int max_address_rounded = (parser->max_address + 3) & ~3; 
    if (max_address_rounded < 128) {
        max_address_rounded = 128;
    }
    
    printf("Arquivo binário gerado corretamente: bytecode.mem\n");

    free(parser);
    free(lexer);
    fclose(input);
    return 0;
}