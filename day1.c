/*
-	only 0-9 allowed
-	no variables allowed
-	if somewhere x/0 is present floating point exception not handled yet :>
		
*/
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define NONE_OF_THESE 0
#define DIGIT 1
#define PLUS 3
#define MINUS 4
#define MULTIPLY 5
#define DIVIDE 6
#define LPAREN 7
#define RPAREN 8

struct info_about_tokens{
	int d_type;
	char value;		//beaware it's in char when value needed for evaluation parse it!
};

struct info_about_tokens stack[30];
int top=-1;

void push(struct info_about_tokens *ptr){

	top++;	
	stack[top].d_type=ptr->d_type;
	stack[top].value=ptr->value;
	
}
struct info_about_tokens* pop(){
	struct info_about_tokens *temp;
	temp=&stack[top];
	top--;
	return temp;
}

struct node{
	int d_type;
	char value;
	struct node* left;
	struct node* right;	
};
struct node *pointer_stack[30];
int AST_top=-1;



int term(struct info_about_tokens *ptr,int token_cnt,int globalindex);
void push_AST(struct node *holding_ptr){
	AST_top++;
	pointer_stack[AST_top]=holding_ptr;
}

struct node* pop_AST(){
		struct node *p=pointer_stack[AST_top];		
		AST_top--;
	return p;
}
struct info_about_tokens new_postfixed[30];//conntains the postfixed array
void gettoken(char *position,struct info_about_tokens *ptr_to_str){
	
	if(isdigit(*position)){
				ptr_to_str->d_type=DIGIT;
				ptr_to_str->value=(*position);
	}
	else if(((*position)=='+')||((*position)=='-')||((*position)=='*')||((*position)=='/')||((*position)=='=')||((*position)==')')||((*position)=='(')){
				switch(*position){
					case '+':
						ptr_to_str->d_type=PLUS;
						break;	
					case '-':
						ptr_to_str->d_type=MINUS;
						break;
					case '*':
						ptr_to_str->d_type=MULTIPLY;
						break;
					case '/':
						ptr_to_str->d_type=DIVIDE;
						break;
					case '(':
						ptr_to_str->d_type=LPAREN;
						break;
					case ')':
						ptr_to_str->d_type=RPAREN;
						break;
				}

				ptr_to_str->value=(*position);	
	}
	else {
				ptr_to_str->d_type=NONE_OF_THESE;
				ptr_to_str->value='N';		//INSERT AS NULL VALUE
	}

}
int validate(int data_type,int identifier){
	if(data_type==identifier)
		return 1;
	else
		return 0;
}
int operator(int data_type){
	//operator:PLUS|MINUS|MULTIPLY|DIVIDE
	if((data_type==PLUS)||(data_type==MINUS)||(data_type==MULTIPLY)||(data_type==DIVIDE))
		return 1;
	else
		return 0;
}
int factor(int data_type,int globalindex){
	//factor:INTEGER|ALPHABET|VARIABLE
	if(data_type==DIGIT)
		return globalindex;	
	else
		return -1;
}

int expr(struct info_about_tokens *ptr,int token_cnt,int globalindex){
	/*
		expr:term ((+|-|*|/)term)*
	*/
	globalindex=term(ptr,token_cnt,globalindex);
	if(globalindex!=-1){
		globalindex++;
			while(1){				
				if(globalindex>=token_cnt){		//does string end here if yes return SUCCESS

					return globalindex;		
				}

				else if((ptr[globalindex].d_type==PLUS)||(ptr[globalindex].d_type==MINUS)||(ptr[globalindex].d_type==MULTIPLY)||(ptr[globalindex].d_type==DIVIDE))
				{
					globalindex++;
					if(globalindex>=token_cnt){//AS STRING ENDS AFTER 2+,SO FAIL
						return (-1);				
					}
					else{//if some term is present after +/*/ check it
						globalindex=term(ptr,token_cnt,globalindex);
						if(globalindex!=-1){//now the 2nd para matches
							globalindex++;
						}
						else
							return (-1);	//-1 for no match
					}
				}
				else if(ptr[globalindex].d_type==RPAREN){
					globalindex--;
					return globalindex;
				}
				else
					return (-1);//other than +-/* and string dosen't end here so FAIL
			}
	}
	else{
		return (-1);
	}

}
int term(struct info_about_tokens *ptr,int token_cnt,int globalindex){
	/*
		term:DIGIT|LPAREN expr RPAREN|
	*/	
	if(globalindex>token_cnt)	
		return -1;
	if(ptr[globalindex].d_type==DIGIT)
		return globalindex;
	else if(ptr[globalindex].d_type==LPAREN){
			globalindex++;
			globalindex=expr(ptr,token_cnt,globalindex);
			if(globalindex==(-1)){
				return (-1);
			}
			else{
				globalindex++;
				if(ptr[globalindex].d_type==RPAREN){
					return globalindex;			
				}
				else{
					return (-1);//no closing brackets				
				}
			}
//			printf("\nshould print '5' or ascii %c now gi is %d",ptr[globalindex].value,globalindex);

	}
	else{
		return (-1);	
	}	
}
void print_info_struct(struct info_about_tokens *loopptr,int token_cnt){
	for(int i=0;i<token_cnt;i++){
		printf("\n(%d %c)",loopptr->d_type,loopptr->value);
		loopptr++;
	}
}
void print_postfix(struct info_about_tokens *loopptr,int cnt_except_paren){
	for(int i=0;i<cnt_except_paren;i++){
		printf("%c",loopptr->value);
		loopptr++;
	}
}
int isoperand(int data_type){
	if(data_type==DIGIT)
		return 1;
	else
		return 0;
}

int isoperator(int data_type){
	if((data_type==PLUS)||(data_type==MINUS)||(data_type==MULTIPLY)||(data_type==DIVIDE))
		return 1;
	else
		return 0;
}
int isparen(int data_type){

	if((data_type==LPAREN)||(data_type==RPAREN))
		return 1;
	else
		return 0;
}

int check_precedence(char operator_val){
	int prec_on_list,prec_on_stack;
	if(operator_val=='+'||operator_val=='-')
		prec_on_list=1;
	else if(operator_val=='*'||operator_val=='/')
		prec_on_list=2;
	
	if(top!=-1){
		switch(stack[top].value){
		case '+':
		case '-':
			prec_on_stack=1;
			break;
		case '*':
		case '/':
			prec_on_stack=2;
			break;
		case '(':
			prec_on_stack=0;//has to have always smaller precedence than others
			break;
		}
	}
	else
		prec_on_stack=-1;


//		printf("OPERATOR VAL: %d %c",operator_val,operator_val);
	if(prec_on_list>prec_on_stack)
		return 1;	//heres a bug need to fix
	else
		return 0;
}


struct info_about_tokens* postfix(struct info_about_tokens *postfix_ptr,int token_cnt){
	struct info_about_tokens *temp_ptr;
	
	//need to create another structure to hold the postfixed value	
	
	temp_ptr=new_postfixed;
	int new_index=0;
	for(int i=0;i<token_cnt;i++){	
		if(isoperand(postfix_ptr->d_type)){			
			new_postfixed[new_index].d_type=postfix_ptr->d_type;		
			new_postfixed[new_index].value=postfix_ptr->value;
			new_index++;
		}
		else if(isoperator(postfix_ptr->d_type)){		
			int ok_to_push=check_precedence(postfix_ptr->value);//ret 1 if striclty greater precedence.
			if(ok_to_push){
				push(postfix_ptr);
			}
			else{
				while((ok_to_push=check_precedence(postfix_ptr->value))!=1){
					struct info_about_tokens *hold=pop();
					new_postfixed[new_index].d_type=hold->d_type;
					new_postfixed[new_index].value=hold->value;
					new_index++;
				}
				push(postfix_ptr);

			}
		}
		else if(isparen(postfix_ptr->d_type)){
			
			switch(postfix_ptr->value){
				case '(':					
					push(postfix_ptr);
					break;
					
				case ')':
										
					while((stack[top].value!='(')){
						struct info_about_tokens *hold=pop();
						new_postfixed[new_index].d_type=hold->d_type;
						new_postfixed[new_index].value=hold->value;
						new_index++;
					}
					struct info_about_tokens *del=pop();//purposefully kept unused need to pop the last element
					break;
			}
		}
		postfix_ptr++;
	}
	while(top>=0){//why >= as need also the value of 0 location of stack
		struct info_about_tokens *hold=pop();
		
		new_postfixed[new_index].d_type=hold->d_type;
		new_postfixed[new_index].value=hold->value;
		new_index++;
	}	

	return temp_ptr;
}

bool
foo (float x)
{
	float y;
	y = floor(x);
	printf ("The number is : %f", y);
	return true;
}

struct node* AST(struct info_about_tokens *ptr_struct,int token_cnt,int cnt_except_paren){
printf("\vnumber %d",cnt_except_paren);


	ptr_struct=postfix(ptr_struct,token_cnt);
	printf("\nPOSTFIX EXPRESSION IS:");
	print_postfix(ptr_struct,cnt_except_paren);

	struct info_about_tokens *loopptr=ptr_struct;
	
	
	for(int i=0;i<cnt_except_paren;i++){

		if(isoperator(loopptr->d_type)){	
			//CREATE A STRUCTURE ,FILL THE VALUES
			struct node *ptr_to_node;
			ptr_to_node=malloc(sizeof(struct node));		
			ptr_to_node->d_type=loopptr->d_type;
			ptr_to_node->value=loopptr->value;//note=the values are in character,while eval. convert to integer
	
			//POP FROM AST ASSIGN TO RIGHT AND THEN LEFT
			ptr_to_node->right=pop_AST();
			ptr_to_node->left=pop_AST();

			push_AST(ptr_to_node);//pass the pointer POINTING TO 'operator'
			
		}
		else{			
			struct node *ptr_to_node;
			ptr_to_node=malloc(sizeof(struct node));
		
			ptr_to_node->d_type=loopptr->d_type;
			ptr_to_node->value=loopptr->value;//note=the values are in character,while eval. convert to integer
			ptr_to_node->left=NULL;
			ptr_to_node->right=NULL;		
			push_AST(ptr_to_node);//pass the pointer to push
		}
	loopptr++;
	}
	
	struct node *ROOT;
	ROOT=pop_AST();
	return ROOT;
}

/*
	I sware :D I copied code from geeksforgeeks,but was fun printing and seeing tree build for first time.
*/
void inorder(struct node *alias_root,int space){
	//printf("\n%c",alias_root->value);
	if(alias_root==NULL)
		return ;
	space+=5;
	inorder(alias_root->right,space);

	printf("\n");
	for(int i=5;i<space;i++)
		printf(" ");
	printf("%c\n",alias_root->value);
	
	inorder(alias_root->left,space);

}
int eval(struct node *alias_root){
	if(!alias_root)
		return 0;
	else if((!alias_root->left)&&(!alias_root->right)){		
		return (alias_root->value-'0');
	}
	int l_value=eval(alias_root->left);
	int r_value=eval(alias_root->right);
	
	if(alias_root->value=='+')
		return (l_value+r_value);
	if(alias_root->value=='-')
		return (l_value-r_value);
	if(alias_root->value=='*')
		return (l_value*r_value);
	if(alias_root->value=='/')
		return (l_value/r_value) ;	
}
int main(void){
	
	char input_expression[30];
	struct info_about_tokens exp_info[30];
	struct node *root;//pointer to the printing structure
	printf("\nENTER THE EXPRESSION");
	scanf("%[^\n]s",input_expression);	//accepts spaces now

	printf("\nTHE EXPRESSION IS: %s",input_expression);		
	
	
	char *position;
	position=input_expression;
	int token_cnt=0;
	int i=0,cnt_except_paren=0;		

	int no_of_paren=0;
	while((*position)!='\0'){

		if((*position=='(')||(*position==')'))
			no_of_paren++;

		if(isspace(*position)){
			position++;
		}

		else{
			gettoken(position,&exp_info[i]);
			token_cnt++;
			i++;
			position++;
		}
	}

	printf("\n\v\t\t######	TOKENIZATION DONE	###### ");
	printf("\nLIST OF TOKENS[0=NONE|1=DIGIT|3=PLUS|4=MINUS|5=MULTIPLY|6=DIVIDE|7=LPAREN|8=RPAREN]");
	for(int i=0;i<token_cnt;i++){
		printf("\n (%d,%c)",exp_info[i].d_type,exp_info[i].value);
	}
	
	cnt_except_paren=token_cnt-no_of_paren;
	
	int globalindex=0;
	int success=expr(exp_info,token_cnt,globalindex);
	printf("\n\v\t\t######	PARSING DONE	###### %d",success);
	if(success!=(-1)){
		printf("\nFORMAT MATCHES");
		printf("\n\v\t\t######	CREATING AST	######");
		root=AST(exp_info,token_cnt,cnt_except_paren);
		printf("\nAST CREATED\nprinting the indorder of AST\n");
		printf("-------------------------------------------");
		inorder(root,0);
		printf("-------------------------------------------");
		int ans=eval(root);
		printf("\n^^^^^^ANSWER IS %d",ans);
	}
	else
		printf("\nFORMAT DOSEN'T MATCHES");
	
}
