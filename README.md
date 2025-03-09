[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/FLG6_3H5)
## Trabalho realizado por
Eduarda Medeiros Silva

## Descrição do Projeto
Este projeto implementa um **assembler** simples que lê um arquivo de entrada contendo código de uma linguagem de montagem (assembly), processa suas instruções e dados, e gera um arquivo binário de bytecode. Ele utiliza um **lexer** (analisador léxico) e um **parser** (analisador sintático) para dividir o código em tokens e realizar a conversão para o formato binário esperado.

## Funcionalidades

- **Lexer**: Converte o código fonte em tokens, identificando instruções, diretivas, variáveis e outros elementos do código.
- **Parser**: Processa os tokens gerados pelo lexer e os traduz para o formato binário.
- **Geração de Arquivo Binário**: O código é traduzido em um formato binário armazenado em um arquivo `.mem`.

## Como Funciona

1. **Entrada**: O código em linguagem de montagem é lido a partir de um arquivo `.txt`. Este arquivo deve estar no formato esperado pelo assembler.
2. **Análise Léxica e Sintática**: O lexer divide o código em tokens e o parser os interpreta conforme as regras definidas.
3. **Processamento**: O parser processa as diretivas `.DATA` e `.CODE`, manipulando variáveis e instruções.
4. **Saída**: O arquivo binário resultante é gravado em um arquivo chamado `bytecode.mem`.

## Limitações Atuais

- **Tratamento de Variáveis**: O código não está considerando o armazenamento adequado das variáveis, conforme discutido em sala de aula. A leitura do nome das variáveis e o mapeamento correto na memória ainda precisam ser implementados.
- **Diretivas**: Atualmente, a diretiva `.ORG` e o tratamento completo de variáveis (como tipo de variável) não estão sendo processados corretamente.
- **Endereço das Variáveis**: O código está colocando o valor no endereço informado pelo código ao invés de ser colocada logo na finalização das instruções. 

## Próximos Passos

- **Implementação de Diretivas**: Adicionar o tratamento da diretiva `.ORG`, permitindo ao programador especificar o endereço de memória onde as instruções serão inicializadas
- **Manipulação Completa de Variáveis**: Implementar a leitura e o armazenamento correto das variáveis, levando em consideração os nomes e tipos.

## Estrutura do Código

1. **Lexer**: Responsável por fazer a leitura do arquivo e dividir o código em tokens. Ele lida com a identificação de mnemônicos, números e diretivas.
2. **Parser**: Processa os tokens gerados pelo lexer. O parser lida com as instruções, diretivas e variáveis e as armazena na memória.
3. **Função Principal (`main`)**: Inicializa o lexer e o parser, chama as funções de leitura e gravação do arquivo e executa a geração do bytecode final.

## Exemplo de Uso

1. Compile o programa utilizando o compilador de sua preferência.
2. Execute o programa passando um arquivo de código fonte `.txt` como argumento:

```bash
./assembler input.txt
```

3. O arquivo `bytecode.mem` será gerado com o bytecode correspondente ao código de entrada.

