struct stack {
    int size;
    void* contents;
};

// places the integer x on top of the stack
// Otherwise , err is set to non - zero value if the stack is full
void push( struct stack * , int x , int * err );

// removes and returns the integer on top of the stack
// Otherwise , err is set to non - zero value if the stack is empty
int pop( struct stack * , int * err );

// returns the integer on top of the stack
// Otherwise , err is set to non - zero value if the stack is empty
int peek( struct stack * , int * err );

// returns non - zero value when empty
int is_empty();

// returns non - zero value when full
int is_full();