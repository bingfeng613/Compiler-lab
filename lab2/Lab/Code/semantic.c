#include "semantic.h"

// 用散列表实现符号表
Entry symbolTable[HASH_SIZE]; // HASH_SIZE = 1024
Entry layersHead; // 另一维度的链表指针

// 初始化符号表
void initSymbolTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        // 散列表的每个槽位都初始化为空指针
        symbolTable[i] = NULL;
    } // 采用open hashing的方式
    // 初始化层次链表头节点
    layersHead = (Entry)malloc(sizeof(Entry_));
    layersHead->hashNext = NULL;
    layersHead->layerNext = NULL;
    // 初始化全局层次节点
    Entry globalLayer = (Entry)malloc(sizeof(Entry_));
    globalLayer->hashNext = NULL;
    globalLayer->layerNext = NULL;
    layersHead->hashNext = globalLayer;
}

// 散列函数PJW
unsigned int hash_pjw(char* name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~HASH_SIZE) {
            val = (val ^ (i >> 12)) & HASH_SIZE;
        }
    }
    return val;
}

// 向符号表中插入符号
void insertSymbol(Entry symbol) {
    // 计算散列值
    unsigned int hash = hash_pjw(symbol->name);
    // 插入对应槽位的链表头
    Entry tail = symbolTable[hash];
    symbolTable[hash] = symbol;
    symbol->hashNext = tail;
    // 插入对应层次的链表头
    Entry currentLayer = layersHead->hashNext;
    tail = currentLayer->layerNext;
    currentLayer->layerNext = symbol;
    symbol->layerNext = tail;
}

// 从符号表中查找符号
Entry findSymbolAll(char* name) {
    unsigned int hash = hash_pjw(name);
    Entry tmp = symbolTable[hash];
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0 && tmp->type != NULL && tmp->type->kind != ENUM_FUNC ) {
            break;
        }
        tmp = tmp->hashNext;
    }
    return tmp;
}

// 查找同一层次的符号
Entry findSymbolLayer(char* name) {
    Entry currentLayer = layersHead->hashNext;
    Entry symbol = currentLayer->layerNext;
    while (symbol != NULL) {
        if (strcmp(symbol->name, name) == 0 && symbol->type->kind != ENUM_FUNC) {
            break;
        }
        symbol = symbol->layerNext;
    }
    return symbol;
}

// 查找函数
Entry findSymbolFunc(char* name) {
    unsigned int hash = hash_pjw(name);
    Entry tmp = symbolTable[hash];
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0 && tmp->type->kind == ENUM_FUNC) {
            break;
        }
        tmp = tmp->hashNext;
    }
    return tmp;
}

// 从符号表中删除符号
void delSymbol(char* name) {
    unsigned int hash = hash_pjw(name);
    Entry pre = NULL;
    Entry tmp = symbolTable[hash];
    while (tmp != NULL) {
        if (strcmp(tmp->name, name) == 0) {
            break;
        }
        pre = tmp;
        tmp = tmp->hashNext;
    }
    // 说明要删除的符号节点是链表的头节点
    if (tmp != NULL && pre == NULL) {
        symbolTable[hash] = tmp->hashNext;
    }
    else if (tmp != NULL && pre != NULL) {
        pre->hashNext = tmp->hashNext;
    }
}

// 插入一个层次
void pushLayer() {
    Entry currentLayer = (Entry)malloc(sizeof(Entry_));
    currentLayer->hashNext = NULL;
    currentLayer->layerNext = NULL;
    Entry tail = layersHead->hashNext;
    layersHead->hashNext = currentLayer;
    currentLayer->hashNext = tail;
}

// 弹出一个层次，同时删除该层次对应的所有符号
void popLayer() {
    Entry currentLayer = layersHead->hashNext;
    layersHead->hashNext = currentLayer->hashNext;
    Entry symbol = currentLayer->layerNext;
    while (symbol != NULL) {
        delSymbol(symbol->name);
        symbol = symbol->layerNext;
    }
}

// 类型等价判断函数
int typeEqual(Type a, Type b) {
    if (a == NULL && b == NULL)
        return 1;
    else if (a == NULL || b == NULL)
        return 0;
    else if (a->kind != b->kind)
        return 0;
    else if (a->kind == ENUM_BASIC) {
        return a->basic == b->basic;
    }
    // 数组等价——基类型和维数相同
    else if (a->kind == ENUM_ARRAY) {
        return typeEqual(a->array.elem, b->array.elem);
    }
    // 结构体类型结构等价
    else if (a->kind == ENUM_STRUCT) {
        FieldList aFields = a->structure->head;
        FieldList bFields = b->structure->head;
        int res = 1;
        while (aFields != NULL && bFields != NULL) {
            if (!typeEqual(aFields->type, bFields->type)) {
                res = 0;
                break;
            }
            aFields = aFields->next;
            bFields = bFields->next;
        }
        if (aFields != NULL || bFields != NULL)
            res = 0;
        return res;
    }
    // 函数类型等价——返回类型、参数个数和参数类型等价
    else if (a->kind == ENUM_FUNC) {
        if (!typeEqual(a->func->returnType, b->func->returnType))
            return 0;
        if (a->func->parmNum != b->func->parmNum)
            return 0;
        FieldList aFields = a->func->head;
        FieldList bFields = b->func->head;
        int res = 1;
        while (aFields != NULL && bFields != NULL) {
            if (!typeEqual(aFields->type, bFields->type)) {
                res = 0;
                break;
            }
            aFields = aFields->next;
            bFields = bFields->next;
        }
        if (aFields != NULL || bFields != NULL)
            res = 0;
        return res;
    }
    else
        return 0;
}


// 以下开始使用语法分析树进行语义解析
void semantic_analyse(Node* root) {
    initSymbolTable();
    Program(root);
    // 检查是否存在已经声明但是未实现的函数
    check(); //check for functions with declaration but no implementations.
}
 // 递归地处理扩展定义列表（例如全局变量和函数声明/定义）
void Program(Node* root) {
    ExtDefList(root->children[0]);
}

// 检查符号表中的每个条目
// 如果一个函数被声明但没有定义，就打印一个错误消息
void check() {
    for (int i = 0; i < HASH_SIZE; i++) {
        if (symbolTable[i] != NULL) {
            Entry entry = symbolTable[i];
            while (entry != NULL) {
                if (entry->type->kind == ENUM_FUNC && entry->type->func->hasDefined == 0) {
                    // 函数声明后没有定义的错误信息
                    printf("Error type 18 at line %d: Undefined function \"%s\".\n", entry->type->func->lineno, 
                    entry->name);
                }
                entry = entry->hashNext;
            }
        }
    }
}

// 递归处理每个扩展定义
void ExtDefList(Node* root) {
    if (root->childNum != 0) {
        ExtDef(root->children[0]);
        ExtDefList(root->children[1]);
    }
}

// 处理单个扩展定义
void ExtDef(Node* root) {
    Type type = Specifier(root->children[0]);
    // 类型错误（结构体重复定义）
    if (type == NULL)
        return;
    // 结构体定义，是结构体，不是匿名类型，域定义没有产生错误
    if (type->kind == ENUM_STRUCT && type->structure->name != "" && type->structure->head != NULL) {
        Entry res = (Entry)malloc(sizeof(Entry_));
        strcpy(res->name, type->structure->name);
        // 需要保证对res->type->kind的改动不会影响到type
        res->type = (Type)malloc(sizeof(Type_));
        res->type->structure = type->structure;
        res->type->kind = ENUM_STRUCT_DEF;
        insertSymbol(res);
    }
    // 全局变量定义 处理变量列表
    if (strcmp(root->children[1]->name, "ExtDecList") == 0) {
        ExtDecList(root->children[1], type);
    }
    // 函数定义
    if (strcmp(root->children[1]->name, "FunDec") == 0) {
        Function func = FunDec(root->children[1]);
        func->returnType = type;
        func->hasDefined = 0;
        Type newType = (Type)malloc(sizeof(Type_));
        newType->kind = ENUM_FUNC;
        newType->func = func;
        Entry sym = findSymbolFunc(func->name);
        // 存在同名函数声明/定义
        if (sym != NULL) {
            // 是函数定义
            if (sym->type->func->hasDefined == 1) {
                // 重复的函数定义
                if (strcmp(root->children[2]->name, "CompSt") == 0)
                    printf("Error type 4 at line %d: Redefined function \"%s\".\n", root->lineno, sym->name);
                // 声明和定义冲突 // 后面是函数声明 
                else if (strcmp(root->children[2]->name, "SEMI") == 0 && !typeEqual(newType, sym->type))
                    printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", root->lineno, sym->name);
                return;
            }
            // 是函数声明
            if (sym->type->func->hasDefined == 0) {
                // 后面是函数定义
                if (strcmp(root->children[2]->name, "CompSt") == 0) {
                    // 定义和声明冲突  类型不匹配
                    if (!typeEqual(newType, sym->type)) {
                        printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", root->lineno, sym->name);
                        return;
                    }
                    // 为已声明的函数添加定义
                    else {
                        sym->type->func->hasDefined = 1;
                        return;
                    }
                }
                else { // 后面是函数声明
                    // 声明和声明冲突
                    if (!typeEqual(newType, sym->type))
                        printf("Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", root->lineno, sym->name);
                    return;
                }
            }
        }
        // 是首次出现的函数声明/定义
        if (strcmp(root->children[2]->name, "SEMI") == 0) {
            Entry res = (Entry)malloc(sizeof(Entry_));
            strcpy(res->name, func->name);
            res->type = newType;
            insertSymbol(res);
        }
        else {
            Entry res = (Entry)malloc(sizeof(Entry_));
            func->hasDefined = 1;
            strcpy(res->name, func->name);
            res->type = newType;
            insertSymbol(res);
            pushLayer();
            CompSt(root->children[2], func->name, func->returnType);
            popLayer();
        }
        return;
    }
    return;
}

// 类型描述符
Type Specifier(Node* root) {
    root = root->children[0];
    if (strcmp(root->name, "TYPE") == 0) {
        Type res = (Type)malloc(sizeof(Type_));
        res->kind = ENUM_BASIC;
        if (strcmp(root->strVal, "int") == 0)
            res->basic = INT_TYPE;
        else if (strcmp(root->strVal, "float") == 0)
            res->basic = FLOAT_TYPE;
        return res;
    }
    else if (strcmp(root->name, "StructSpecifier") == 0)
        return StructSpecifier(root);
}

// 全局变量名称列表
void ExtDecList(Node* root, Type type) {
    if (root->childNum == 1) // 只有一个子节点 直接进行处理
        VarDec(root->children[0], type, ENUM_VAR);
    else {
        VarDec(root->children[0], type, ENUM_VAR);
        // 递归处理剩下的子节点
        ExtDecList(root->children[2], type);
    }
}

// 函数名和参数列表（不检查错误）
Function FunDec(Node* root) {
    // 构造一个新的函数描述符
    Function res = (Function)malloc(sizeof(Function_));
    strcpy(res->name, root->children[0]->strVal);
    res->parmNum = 0;
    res->lineno = root->lineno;
    // 解析函数名称和参数列表 并将其存储在函数描述符中
    if (root->childNum == 3)
        res->head = NULL;
    else {
        pushLayer();
        res->head = VarList(root->children[2]);
        popLayer();
        FieldList tmp = res->head;
        while (tmp != NULL) {
            res->parmNum += 1;
            tmp = tmp->next;
        }
    }
    // 返回函数描述符
    return res;
}
// 处理复合语句 通常是函数体
void CompSt(Node* root, char* funcName, Type reType) {
    // 先把函数参数存进符号表
    Type type = reType;
    if (funcName != NULL) {
        Entry sym = findSymbolFunc(funcName);
        FieldList parms = sym->type->func->head;
        while (parms != NULL) {
            Entry parm = (Entry)malloc(sizeof(Entry_));
            strcpy(parm->name, parms->name);
            parm->type = parms->type;
            insertSymbol(parm);
            parms = parms->next;
        }
        type = sym->type->func->returnType;
    }
    // 处理函数体内的定义列表和语句列表
    DefList(root->children[1], ENUM_VAR);
    StmtList(root->children[2], type);
    return;
}

// 处理结构体描述符
Type StructSpecifier(Node* root) {
    // 构造一个新的结构体描述符
    Type res = (Type)malloc(sizeof(Type_));
    res->kind = ENUM_STRUCT;
    res->structure = (Structure)malloc(sizeof(Structure_));
    for (int i = 0; i < root->childNum; i++) {
        Node* child = root->children[i];
        if (strcmp(child->name, "OptTag") == 0) {
            if (child->childNum == 0)
                strcpy(res->structure->name, "");
            else {
                // 结构体名字全局唯一
                Entry sym = findSymbolAll(child->children[0]->strVal);
                // 结构体的名字与前面定义过的结构体或变量的名字重复
                if (sym != NULL) {
                    printf("Error type 16 at line %d: Duplicated name \"%s\".\n", child->lineno, child->children[0]->strVal);
                    return NULL;
                }
                strcpy(res->structure->name, child->children[0]->strVal);
            }
        }
        // 这里是要使用一个已经定义的结构体类型，可能会产生错误
        else if (strcmp(child->name, "Tag") == 0) {
            Entry sym = findSymbolAll(child->children[0]->strVal);
            // 该结构体的名称不在符号表中，或查找出的条目不属于结构体定义类型
            if (sym == NULL || sym->type->kind != ENUM_STRUCT_DEF) {
                printf("Error type 17 at line %d: Undefined struct \"%s\".\n", child->lineno, child->children[0]->strVal);
                return NULL;
            }
            res->structure = sym->type->structure;
            return res;
        }
        // 定义新的结构体类型 处理结构体内的定义列表
        else if (strcmp(child->name, "DefList") == 0) {
            pushLayer();
            res->structure->head = DefList(child, ENUM_FIELD);
            popLayer();
            return res;
        }
    }
}

// 变量定义
FieldList DefList(Node* root, IdType class) {
    // 没有子节点 返回NULL
    if (root->childNum == 0)
        return NULL;
    else {
        // 解析第一个子节点
        FieldList res = Def(root->children[0], class);
        // 递归地处理剩余的子节点
        if (res == NULL)
            res = DefList(root->children[1], class);
        else {
            // 需要挂在最后面
            FieldList tmp = res;
            while (tmp->next != NULL) tmp = tmp->next;
            tmp->next = DefList(root->children[1], class);
        }
        return res;
    }
}
// 解析单个变量或字段定义
FieldList Def(Node* root, IdType class) {
    Type type = Specifier(root->children[0]);
    FieldList res = DecList(root->children[1], type, class);
    return res;
}

// 解析变量或字段声明的列表
FieldList DecList(Node* root, Type type, IdType class) {
    FieldList res = Dec(root->children[0], type, class);
    if (root->childNum == 3) {
        if (res == NULL)
            res = DecList(root->children[2], type, class);
        else {
            FieldList tmp = res;
            while (tmp->next != NULL) tmp = tmp->next;
            tmp->next = DecList(root->children[2], type, class);
        }
    }
    return res;
}

// 解析一个变量或字段的声明
FieldList Dec(Node* root, Type type, IdType class) {
    FieldList res = VarDec(root->children[0], type, class);
    // 错误：在定义结构体时对域进行初始化
    if (class == ENUM_FIELD && res != NULL && root->childNum == 3) {
        printf("Error type 15 at line %d: Initialized field \"%s\".\n", root->lineno, res->name);
        return NULL;
    }
    // 对于变量，如果进行了初始化，检查类型匹配
    if (class == ENUM_VAR && res != NULL && root->childNum == 3) {
        Type right = Exp(root->children[2]);
        if (right != NULL && !typeEqual(right, type)) {
            printf("Error type 5 at line %d: Type mismatched.\n", root->lineno);
            return NULL;
        }
    }
    return res;
}

// 解析变量或字段的具体定义
FieldList VarDec(Node* root, Type type, IdType class) {
    // 对于简单变量，直接查找其名称是否已在当前作用域或全局作用域中定义
    if (root->childNum == 1) {
        Entry sym = findSymbolLayer(root->children[0]->strVal);
        Entry symA = findSymbolAll(root->children[0]->strVal);
        // 域名/变量名重复定义或与结构体定义重复
        if (sym != NULL || (symA != NULL && symA->type->kind == ENUM_STRUCT_DEF)) {
            if (class == ENUM_FIELD)
                printf("Error type 15 at line %d: Redefined field \"%s\".\n", root->lineno, root->children[0]->strVal);
            if (class == ENUM_VAR)
                printf("Error type 3 at line %d: Redefined variable \"%s\".\n", root->lineno, root->children[0]->strVal);
            return NULL;
        }
        FieldList res = (FieldList)malloc(sizeof(FieldList_));
        strcpy(res->name, root->children[0]->strVal);
        res->type = type;
        // 域也要加符号表
        Entry tmp = (Entry)malloc(sizeof(Entry_));
        strcpy(tmp->name, root->children[0]->strVal);
        tmp->type = type;
        insertSymbol(tmp); 
        return res;
    }
    // 对于数组类型，解析数组大小并递归地处理元素类型
    // 数组
    else {
        Type newType = (Type)malloc(sizeof(Type_));
        newType->kind = ENUM_ARRAY;
        newType->array.elem = type;
        newType->array.size = root->children[2]->intVal;
        return VarDec(root->children[0], newType, class);
    }
}

// 参数列表
// 首先解析第一个参数，然后递归地处理剩余的参数
FieldList VarList(Node* root) {
    FieldList res = ParamDec(root->children[0]);
    if (root->childNum == 3) {
        res->next = VarList(root->children[2]);
    }
    return res;
}

// 函数参数
// 解析单个函数参数
FieldList ParamDec(Node* root) {
    Type type = Specifier(root->children[0]);
    return VarDec(root->children[1], type, ENUM_FIELD);
}

// 
// 语句列表
void StmtList(Node* root, Type reType) {
    if (root->childNum == 2) {
        Stmt(root->children[0], reType);
        StmtList(root->children[1], reType);
    }
    return;
}

// 进行个别语句的处理
void Stmt(Node* root, Type reType) {
    if (strcmp(root->children[0]->name, "RETURN") == 0) {
        Type type = Exp(root->children[1]);
        if (!typeEqual(reType, type))
            printf("Error type 8 at line %d: Type mismatched for return.\n", root->lineno);
    }
    else if (strcmp(root->children[0]->name, "CompSt") == 0) {
        pushLayer();
        CompSt(root->children[0], NULL, reType);
        popLayer();
    }
    else if (strcmp(root->children[0]->name, "Exp") == 0) {
        Exp(root->children[0]);
    }
    else if (strcmp(root->children[0]->name, "WHILE") == 0) {
        Exp(root->children[2]);
        Stmt(root->children[4], reType);
    }
    else if (strcmp(root->children[0]->name, "IF") == 0) {
        Exp(root->children[2]);
        Stmt(root->children[4], reType);
        if (root->childNum == 7)
            Stmt(root->children[6], reType);
    }
    return;
}

// 分析表达式
Type Exp(Node* root) {
    if (strcmp(root->children[0]->name, "Exp") == 0) {
        // 对结构体使用.操作符
        if (strcmp(root->children[1]->name, "DOT") == 0) {
            Type res = Exp(root->children[0]);
            if (res != NULL) {
                if (res->kind != ENUM_STRUCT) {
                    printf("Error type 13 at line %d: Illegal use of \".\".\n", root->lineno);
                    return NULL;
                }
                char field[32];
                strcpy(field, root->children[2]->strVal);
                // 检测域名是否有效
                FieldList head = res->structure->head;
                Type ans = NULL;
                while (head != NULL) {
                    if (strcmp(field, head->name) == 0) {
                        ans = head->type;
                        break;
                    }
                    head = head->next;
                }
                // 域名不存在
                if (ans == NULL) {
                    printf("Error type 14 at line %d: Non-existed field \"%s\".\n", root->lineno, field);
                    return NULL;
                }
                return ans;
            }
            return res;
        }
        // 数组取地址操作
        else if (strcmp(root->children[1]->name, "LB") == 0) { // children[1]如果是[
            Type pre = Exp(root->children[0]); // [ 前的内容
            if (pre != NULL) {
                // 数组[]前面的类型不正确
                if (pre->kind != ENUM_ARRAY) { 
                    printf("Error type 10 at line %d: Expect an array before [...].\n", root->lineno);
                    return NULL;
                }
                Type index = Exp(root->children[2]); // ]后的内容
                if (index == NULL || index->kind != ENUM_BASIC || index->basic != INT_TYPE) {
                    printf("Error type 12 at line %d: Expect an integer in [...].\n", root->lineno);
				    return NULL;
                }
                return pre->array.elem;
            }
            return pre;
        }
        // 赋值操作
        else if (strcmp(root->children[1]->name, "ASSIGNOP") == 0) {
            // 左值只有三种情况
            // 1、变量
            // 2、域
            // 3、数组元素
            Node* left = root->children[0];
            Node* right = root->children[2];
            Type leftType = NULL;
            Type rightType = Exp(right);
            if ((left->childNum == 1 && strcmp(left->children[0]->name, "ID") == 0) ||
				(left->childNum == 4 && strcmp(left->children[1]->name, "LB") == 0) ||
			    (left->childNum == 3 && strcmp(left->children[1]->name, "DOT") == 0))
				leftType = Exp(left);
			else {
				printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", root->lineno);
                return NULL;
            }
            if (leftType != NULL && rightType != NULL && !typeEqual(leftType, rightType)) {
                printf("Error type 5 at line %d: Type mismatched for assignment.\n", root->lineno);
                return NULL;
            }
            return leftType;
        }
        // 普通二元运算操作
        else {
            Type pre = Exp(root->children[0]);
            Type aft = Exp(root->children[2]);
            if (pre == NULL || aft == NULL)
                return NULL;
            if (!typeEqual(pre, aft)) {
                printf("Error type 7 at line %d: Type mismatched for operands.\n", root->lineno);
				return NULL;
            }
            if (strcmp(root->children[1]->name, "RELOP") == 0) {
                Type res = (Type)malloc(sizeof(Type_));
                res->kind = ENUM_BASIC;
                res->basic = INT_TYPE;
                return res;  
            }
            return pre;
        }
    }
    else if (strcmp(root->children[0]->name, "LP") == 0) {
        Type res = Exp(root->children[1]);
        return res;
    }
    else if (strcmp(root->children[0]->name, "MINUS") == 0) {
        Type res = Exp(root->children[1]);
        // 如果res为NULL应该是Exp有错，这里就不再报连锁错误
        if (res != NULL)
            if (res->kind != ENUM_BASIC) {
                printf("Error type 7 at line %d: Operands type mismatched.\n", root->lineno);
                return NULL;
            }
        return res;
    }
    else if (strcmp(root->children[0]->name, "NOT") == 0) {
        Type res = Exp(root->children[1]);
        if (res != NULL)
            if (res->kind != ENUM_BASIC || res->basic != INT_TYPE) {
                printf("Error type 7 at line %d: Operands type mismatched.\n", root->lineno);
                return NULL;
            }
        return res;
    }
    else if (strcmp(root->children[0]->name, "ID") == 0) {
        // ID是一个变量
        if (root->childNum == 1) {
            Entry sym = findSymbolAll(root->children[0]->strVal);
            // 使用不存在的变量
            if (sym == NULL) {
                printf("Error type 1 at line %d: Undefined variable \"%s\".\n", root->lineno, root->children[0]->strVal);
                return NULL;
            }
            return sym->type;
        }
        // ID是一个函数名
        else {
            Entry sym = findSymbolFunc(root->children[0]->strVal);
            if (sym == NULL) {
                sym = findSymbolAll(root->children[0]->strVal);
                // 对普通变量使用()操作符
                if (sym != NULL) {
                    printf("Error type 11 at line %d: \"%s\" is not a function.\n", root->lineno, sym->name);
                    return NULL;
                }
                // 使用不存在的函数
                else {
                    printf("Error type 2 at line %d: Undefined function \"%s\".\n", root->lineno, root->children[0]->strVal);
                    return NULL;
                }
            }
            // 使用未定义的函数
            if (sym->type->func->hasDefined == 0) {
                printf("Error type 2 at line %d: Undefined function \"%s\".\n", root->lineno, sym->name);
                return NULL;
            }
            // 提供的参数
            FieldList args = NULL;
            FieldList args_ = NULL;
            // 函数应有的参数
            FieldList realArgs = sym->type->func->head;
            // 函数有参数
            if (root->childNum == 4) {
                args = Args(root->children[2]);
                args_ = args;
            }
            int flag = 1;
            while (args != NULL && realArgs != NULL) {
                if (!typeEqual(args->type, realArgs->type)) {
                    flag = 0;
                    break;
                }
                args = args->next;
                realArgs = realArgs->next;
            }
            if (args != NULL || realArgs != NULL)
                flag = 0;
            if (flag == 0) {
                printf("Error type 9 at line %d: The method \"%s(", root->lineno, sym->name);
				printArgs(sym->type->func->head);
				printf(")\" is not applicable for the arguments \"(");
                printArgs(args_);
                printf(")\".\n");
            }
            return sym->type->func->returnType;
        }
    }
    else if (strcmp(root->children[0]->name, "INT") == 0) {
        Type res = (Type)malloc(sizeof(Type_));
        res->kind = ENUM_BASIC;
        res->basic = INT_TYPE;
        return res;
    }
    else if (strcmp(root->children[0]->name, "FLOAT") == 0) {
        Type res = (Type)malloc(sizeof(Type_));
        res->kind = ENUM_BASIC;
        res->basic = FLOAT_TYPE;
        return res;
    }
}

FieldList Args(Node* root) {
    FieldList res = (FieldList)malloc(sizeof(FieldList_));
    res->type = Exp(root->children[0]);
    if (root->childNum == 3)
        res->next = Args(root->children[2]);
    return res;
}


// 打印参数类型列表
void printArgs(FieldList head) {
    if (head == NULL)
        return;
    if (head->next == NULL) {
        printType(head->type);
        return;
    }
    printType(head->type);
    printf(", ");
    printArgs(head->next);
}

// 打印类型
void printType(Type type) {
	if (type->kind == ENUM_BASIC) {
        if (type->basic == INT_TYPE)
            printf("int");
        else
            printf("float");
    }
	else if (type->kind == ENUM_STRUCT)
		printf("struct %s", type->structure->name);
	else if (type->kind == ENUM_ARRAY) {
		printType(type->array.elem);
		printf("[]");
	}
    else if (type->kind = ENUM_FUNC)
        printf("func %s", type->func->name);
    else if (type->kind == ENUM_STRUCT_DEF)
        printf("struct definition %s", type->structure->name);
}