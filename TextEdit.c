#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEXT 256                //ขนาดสูงสุดของข้อความ (รวม '0')

// ---------- Node ของ Linked List (ใช้ทำ Stack) ---------- //
// แต่ละ Node คือ "สถานะ" หนึ่งครั้งของ editor ที่เก็บไว้ให้ undo
typedef struct Node {
    char text[MAX_TEXT];        // ข้อความทั้งหมด ณ สถานะนั้น
    int lastStart;              // ตำแหน่งเริ่มของชุดล่าสุดในสถานะนั้น
    struct Node *next;          // Node ถัดไป (สถานะที่เก่ากว่า)
} Node;

// ---------- Stack แบบ Linked List ---------- //
typedef struct {
    Node *top;                  // Node บนสุด = สถานะล่าสุดที่เก็บไว้
} Stack;

// เริ่มต้น Stack ให้ว่าง 
void initStack(Stack *s) {
    s->top = NULL;
}

//เช็กว่า Stack ว่างหรือไม่
int isEmpty(Stack *s) {
    return s->top == NULL;
}

//เพิ่มสถานะ (ข้อความ + lastStart) ลงบนสุดของ Stack
void push(Stack *s, const char *text, int lastStart) {
    Node *newNode = (Node *)malloc(sizeof(Node));   // จองหน่วยความจำให้ Node ใหม่
    if (newNode == NULL) {
        printf("Memory error!\n");
        return;
    }
    strcpy(newNode->text, text);
    newNode->lastStart = lastStart;
    newNode->next = s->top;     // ชี้ไปที่ Node เดิมที่เคยอยู่บนสุด
    s->top = newNode;           // Node ใหม่กลายเป็นบนสุด
}

// ดึงสถานะบนสุดออกมาเก็บใน out / outStart แล้วลบ Node ทิ้ง
// คืน 1 ถ้าสำเร็จ, 0 ถ้า Stack ว่าง
// outStart ส่ง NULL ได้ ถ้าไม่ต้องการค่า lastStart
int pop(Stack *s, char *out, int *outStart) {
    if (isEmpty(s)) return 0;
    Node *temp = s->top;
    strcpy(out, temp->text);
    if (outStart != NULL) *outStart = temp->lastStart;
    s->top = temp->next;        // เลื่อนบนสุดลงมา
    free(temp);                 // คืนหน่วยความจำ
    return 1;
}

void clearStack(Stack *s) {
    char dummy[MAX_TEXT];
    while (pop(s, dummy, NULL));
}

char current[MAX_TEXT] = "";    // ข้อความปัจจุบัน
int lastStart = 0;              // ตำแหน่งเริ่มของข้อความชุดล่าสุด
Stack undoStack;                // เก็บประวัติสถานะไว้ให้ undo

void addText(const char *input) {
    if (strlen(current) + strlen(input) >= MAX_TEXT) {
        printf("Text too long!\n");             // ยาวเกิน -> ไม่ทำอะไร ไม่เก็บ undo
        return;
    }
    push(&undoStack, current, lastStart);   //เก็บทั้งข้อความและ lastStart เดิม
    lastStart = strlen(current);            //ชุดใหม่เริ่มที่จุดท้ายสุดของข้อความเดิม
    strcat(current, input);
}

// แทนที่เฉพาะข้อความชุดล่าสุดด้วยข้อความใหม่
void editText(const char *input) {
    if (lastStart + strlen(input) >= MAX_TEXT) {
        printf("Text too long!\n");
        return;
    }
    push(&undoStack, current, lastStart);
    current[lastStart] = '\0';              //ตัดชุดล่าสุดทิ้ง
    strcat(current, input);                 //ต่อชุดใหม่แทน (lastStart เท่าเดิม)
}

//ย้อนกลับไปสถานะก่อนหน้า
void undo(void) {
    if (isEmpty(&undoStack)) {
        printf("Nothing to undo.\n");
        return;
    }
    pop(&undoStack, current, &lastStart);   //คืนทั้งข้อความและ lastStart
    printf("Undo done.\n");
}

//แสดงข้อความปัจจุบัน
void show(void) {
    printf("Current text: \"%s\"\n", current);
}


int main(void) {
    int choice;                 //เมนูที่ผู้ใช้เลือก
    char input[MAX_TEXT];       //ข้อความที่ผู้ใช้พิมพ์

    initStack(&undoStack);

    /* วนแสดงเมนูจนกว่าผู้ใช้เลือก 0 */
    do {
        printf("\n===== Text Editor =====\n");
        printf("1. Add text\n");
        printf("2. Edit last text\n");
        printf("3. Undo\n");
        printf("4. Show\n");
        printf("0. Exit\n");
        printf("Choose: ");
        if (scanf("%d", &choice) != 1) break;   //พิมพ์ไม่ใช่ตัวเลข -> ออกจากลูป
        getchar();                              //ทิ้ง '\n' ที่ค้างอยู่

        switch (choice) {
        case 1:
            printf("Enter text to add: ");
            fgets(input, MAX_TEXT, stdin);
            input[strcspn(input, "\n")] = '\0';     //ตัด '\n' ท้ายบรรทัดออก
            addText(input);
            break;
        case 2:
            printf("Enter new text for the last part: ");
            fgets(input, MAX_TEXT, stdin);
            input[strcspn(input, "\n")] = '\0';
            editText(input);
            break;
        case 3: undo(); break;
        case 4: show(); break;
        case 0: printf("Bye!\n"); break;
        default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    clearStack(&undoStack);     //คืนหน่วยความจำก่อนจบ
    return 0;
}
