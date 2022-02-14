#include <stdio.h>
#include <stdlib.h>
#include <math.h>




struct Token
{
    char type; //N, U, O, B
    void* token;
};
struct numberToken
{
    float value;
};
struct unaryOperandToken
{
    char value; //!
};
struct binaryOperandToken
{
    char value; //*, -, +, /, ^
};
struct bracketToken
{
    char value; //'(', ')'
};

void newToken(struct Token* tokenArr, int index, char tokenType, char stringData[], int beginning);
char getTokenType(char a);
int getBinaryPrecedence(char binaryOperand);
void printTokens(struct Token* tokenArr, int size);
float binaryResult(struct numberToken left, struct binaryOperandToken operand, struct numberToken right);
void solveBasicExpression(struct Token arr[], int size);
float calculate(struct Token arr[], int size);



int main(int argc, char* args[])
{


    char inputString[256];


    while (1)
    {
        printf("Enter Expression: ");
        int stringLength = 0;
        fgets(inputString, 256, stdin);
        for (; inputString[stringLength] != '\n'; ++stringLength);

        if (!stringLength) continue;



        //Check for invalid brackets
        int bracketCounter = 0;
        for (int i = 0; i < stringLength; ++i)
        {
            if (inputString[i] == '(')
            {
                bracketCounter++;
            }
            else if (inputString[i] == ')')
            {
                bracketCounter--;
                if (bracketCounter < 0) goto InvalidExpression;
            }
        }
        if (bracketCounter) goto InvalidExpression;


        //Allocate maximum possible
        struct Token* tokens = (struct Token*)malloc(stringLength * sizeof(struct Token));



        int tokenCount = 0;

        int currentlyNumberFlag = 0;
        for (int i = 0; i < stringLength; ++i)
        {
            char tokenType = getTokenType(inputString[i]);
            if (tokenType == 'B' || tokenType == 'O' || tokenType == 'U')
            {
                newToken(tokens, tokenCount, tokenType, inputString, i);
                tokenCount++;
                currentlyNumberFlag = 0;
            }
            else if (tokenType == 'N')
            {
                if (currentlyNumberFlag) continue;
                newToken(tokens, tokenCount, tokenType, inputString, i);
                tokenCount++;
                currentlyNumberFlag = 1;
            }
            //Whitespace
            else if (inputString[i] == ' ' || inputString[i] == '\t')
                continue;
            else
            {
                goto InvalidExpression;
            }
        }

        tokens = realloc(tokens, tokenCount * sizeof(struct Token));


        //Check validity and if invalid continue;
        if (tokens[0].type == 'O' || tokens[tokenCount - 1].type == 'O')
        {
            goto InvalidExpression;
        }

        for (int i = 1; i < tokenCount; ++i)
        {
            if (tokens[i - 1].type == 'O' && tokens[i].type == 'O')
            {
                goto InvalidExpression;
            }
        }


        //Make sure there is at least one number token
        int isNumberToken = 0;
        for (int i = 0; i < tokenCount; ++i)
        {
            if (tokens[i].type == 'N')
            {
                isNumberToken = 1;
                break;
            }
        }

        if (isNumberToken == 0)
        {
            for (int i = 0; i < tokenCount; ++i)
            {
                free(tokens[i].token);
                free(tokens);
                goto InvalidExpression;
            }
        }


        //printTokens(tokens, tokenCount);
        printf("%f\n\n", calculate(tokens, tokenCount)); //This frees everything but token[0]
        free(tokens[0].token);
        free(tokens);
        continue;

    InvalidExpression:
        printf("Invalid Expression!\n\n");
    }





    return 0;
}







void newToken(struct Token* tokenArr, int index, char tokenType, char stringData[], int beginning)
{
    tokenArr[index].type = tokenType;

    if (tokenType == 'N')
    {
        tokenArr[index].token = malloc(sizeof(struct numberToken));

        char* str = stringData + beginning;

        ((struct numberToken*)tokenArr[index].token)->value = strtof(str, NULL);
    }
    else if (tokenType == 'U')
    {
        tokenArr[index].token = malloc(sizeof(struct unaryOperandToken));
        ((struct unaryOperandToken*)tokenArr[index].token)->value = stringData[beginning];
    }
    else if (tokenType == 'O')
    {
        tokenArr[index].token = malloc(sizeof(struct binaryOperandToken));
        ((struct binaryOperandToken*)tokenArr[index].token)->value = stringData[beginning];
    }
    else if (tokenType == 'B')
    {
        tokenArr[index].token = malloc(sizeof(struct bracketToken));
        ((struct bracketToken*)tokenArr[index].token)->value = stringData[beginning];
    }
    
}

char getTokenType(char a)
{
    if ((a >= '0' && a <= '9') || a == '.') return 'N';
    else if (a == '^' || a == '/' || a == '*' || a == 'x' || a == '+' || a == '-') return 'O';
    else if (a == '!') return 'U';
    else if (a == '(' || a == ')') return 'B';
    else return 'X'; //This isn't supposed to happen
}

int getBinaryPrecedence(char binaryOperand)
{
    if (binaryOperand == '-') return 1;
    else if (binaryOperand == '+') return 1;
    else if (binaryOperand == '*' || binaryOperand == 'x') return 2;
    else if (binaryOperand == '/') return 3;
    else if (binaryOperand == '^') return 4;
    else return 0; //This isn't supposed to happen
}

void printTokens(struct Token* tokenArr, int size)
{
    printf("Tokens:\n");
    for (int i = 0; i < size; ++i)
    {
        printf("\tToken Type: %c\n", tokenArr[i].type);
        if (tokenArr[i].type == 'N')
        {
            printf("\tToken Value: %f\n\n", ((struct numberToken*)tokenArr[i].token)->value);
        }
        else
        {
            printf("\tToken Value: %c\n\n", ((struct binaryOperandToken*)tokenArr[i].token)->value); //This is bad, but it works because binaryOperand token has the same 'value' attribute as all the others 
        }
        
    }
}

float binaryResult(struct numberToken left, struct binaryOperandToken operand, struct numberToken right)
{
    if (operand.value == '^') return powf(left.value, right.value);
    else if (operand.value == '/') return left.value / right.value;
    else if (operand.value == '*' || operand.value == 'x') return left.value * right.value;
    else if (operand.value == '+') return left.value + right.value;
    else if (operand.value == '-') return left.value - right.value;
    else return 0.0f; //Not supposed to happen
}

//No brackets in here
void solveBasicExpression(struct Token arr[], int size)
{

    //Multiply two numbers next to each other (EG 3 4 becomes 3*4. 2 ^ 3 4 becomes 2^12, so lets say 3 4 is (3*4)
    for (int i = 1; i < size; ++i)
    {
        if (arr[i].type == 'N' && arr[i - 1].type == 'N')
        {
            ((struct numberToken*)arr[i - 1].token)->value *= ((struct numberToken*)arr[i].token)->value;
            for (int j = i; j < size - 1; ++j)
            {
                arr[j] = arr[j + 1];
            }
            size -= 1;
        }
    }

    //printTokens(arr, size);
    int binaryOperandCount = 0;
    for (int i = 0; i < size; ++i)
    {
        if (arr[i].type == 'O') binaryOperandCount++;
    }

    for (binaryOperandCount; binaryOperandCount--;)
    {

        int highestLocation = 0;
        int highestPrecedence = 0;
        for (int i = 1; i < size; ++i)
        {
            if (arr[i].type == 'O') //Don't forget unary
            {
                if (highestPrecedence <= getBinaryPrecedence(((struct binaryOperandToken*)arr[i].token)->value))
                {
                    highestPrecedence = getBinaryPrecedence(((struct binaryOperandToken*)arr[i].token)->value);
                    highestLocation = i;
                }
            }
        }

        float result = binaryResult(*((struct numberToken*)arr[highestLocation - 1].token), *((struct binaryOperandToken*)arr[highestLocation].token), *((struct numberToken*)arr[highestLocation + 1].token));


        //Replace Number BinaryOperand Number with Number (result)

        ((struct numberToken*)arr[highestLocation - 1].token)->value = result;

        for (int i = highestLocation + 2; i < size; ++i)
        {
            arr[i - 2] = arr[i];
        }
        size -= 2;

        //printTokens(arr, size);


        //Multiply two numbers next to each other (EG 3 4 becomes 3*4. 2 ^ 3 4 becomes 2^12, so lets say 3 4 is (3*4)
        for (int i = 1; i < size; ++i)
        {
            if (arr[i].type == 'N' && arr[i - 1].type == 'N')
            {
                ((struct numberToken*)arr[i - 1].token)->value *= ((struct numberToken*)arr[i].token)->value;
                for (int j = i; j < size - 1; ++j)
                {
                    arr[j] = arr[j + 1];
                }
                size -= 1;
            }
        }
    }

    //printf("%f\n", ((struct numberToken*)arr[0].token)->value);
    //return ((struct numberToken*)arr[0].token)->value;
}

float calculate(struct Token arr[], int size)
{
    //Ignore brackets for now

    int originalSize = size;

    //Find highest inbrackketed value (so +1 every '(', -1 every ')', keep track of highest and location)
    //Find number of tokens in it and solve basic expression, since the result must be a number we can shift everything at the end

    while (1)
    {
        int currentBeginningLocation = 0;
        int currentHighestNest = 0;
        int currentNest = 0;
        int bracketFlag = 0;
        for (int i = 0; i < size; ++i)
        {
            if (arr[i].type == 'B')
            {
                currentNest += (((struct bracketToken*)arr[i].token)->value) == '(' ? 1 : -1;
                if (currentNest > currentHighestNest)
                {
                    currentHighestNest = currentNest;
                    currentBeginningLocation = i;
                }
                bracketFlag = 1;
            }
        }

        if (!bracketFlag)
        {
            solveBasicExpression(arr, size);
            return ((struct numberToken*)arr[0].token)->value;
        }

        currentBeginningLocation++;
        int expressionSize = size;
        for (int i = currentBeginningLocation; i < size; ++i)
        {
            if (arr[i].type == 'B')
            {
                expressionSize = i - currentBeginningLocation;
                break;
            }
        }
        //printf("%d, %d\n", currentBeginningLocation, expressionSize);
       
        struct Token* subArr = arr + currentBeginningLocation;
        //This modifies arr
        solveBasicExpression(subArr, expressionSize);

        //Turn B N _ _ _ B into N
        arr[currentBeginningLocation - 1] = arr[currentBeginningLocation];

        for (int i = currentBeginningLocation; i < size - expressionSize - 1; ++i)
        {
            arr[i] = arr[i + expressionSize + 1];
        }
        size -= (expressionSize + 1);
        //printTokens(arr, size);
    }

    

    for (int i = 1; i < originalSize; ++i)
        free(arr[i].token);

    return ((struct numberToken*)arr[0].token)->value;

}



