/*ZAMFIR IRINA-MARIA - 313CC*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct page
{
    int id;
    char url[50];
    char *desc;
};
typedef struct page page;

struct nod
{
    page *Page;
    struct nod *next;
};
typedef struct nod nod;

struct stack
{
    nod *head;
};
typedef struct stack stack;

struct tab
{
    int id;
    page *currentPage;
    stack *backward;
    stack *forward;
};
typedef struct tab tab;

struct tablist
{
    tab *Tab;
    struct tablist *next;
    struct tablist *prev;
};
typedef struct tablist tablist;

struct browser
{
    tab *currentTab;
    tablist *list;
};
typedef struct browser browser;

/*functie care verifica daca un caracter este cifra sau nu*/
int isNumber(char ch)
{
    if ('0' <= ch && ch <= '9')
        return 1;
    else
        return 0;
}

int construct_number(char s[])
{
    int i = 0, nr = 0;
    /*parcurg sirul de caractere pana cand gasesc o cifra*/
    while (isNumber(s[i]) == 0 && s[i] != '\0')
        i++;

    /*stiind ca cifrele se afla la finalul sirului de caractere, il parcug pana
    la final si construiesc numarul*/
    while (s[i] != '\0')
    {
        nr = nr * 10 + (s[i] - '0');
        i++;
    }

    /*returnez numarul*/
    return nr;
}

/*functie care verifica daca stiva este goala sau nu*/
int isEmpty(stack *s)
{
    return s->head == NULL;
}

/*functie care pune o pagina intr-o stiva*/
void push(stack *s, page *Page)
{
    nod *newnode = (nod *)malloc(sizeof(nod));
    newnode->Page = Page;
    newnode->next = s->head;
    s->head = newnode;
}

/*functie care scoate ultima pagina adaugata intr-o stiva*/
void pop(stack *s)
{
    if (isEmpty(s))
        return;

    nod *temp = s->head;
    s->head = s->head->next;
    free(temp);
}

page *init_new_page(int id, char url[], char d[])
{
    /*declar si aloc memorie pentru un noua pagina si ii dau id-ului valoarea
    variabilei id trimisa ca parametru*/
    page *Page = (page *)malloc(sizeof(page));
    Page->id = id;
    /*copiez in campul url, url-ul paginii*/
    strcpy(Page->url, url);
    int l = strlen(d);
    /*aloc memorie pentru descierea paginii*/
    Page->desc = (char *)malloc((l + 1) * sizeof(char));
    /*copiez in campul desc, descrierea paginii*/
    strcpy(Page->desc, d);
    Page->desc[l] = '\0';
    /*returnez pagina creata*/
    return Page;
}

tab *init_new_tab(int id)
{
    /*declar si aloc memorie pentru un nou tab si ii dau id-ului valoarea
    variabilei id trimisa ca parametru*/
    tab *Tab = (tab *)malloc(sizeof(tab));
    Tab->id = id;
    /*initializez pagina curenta cu id 0*/
    Tab->currentPage = init_new_page(0, "https://acs.pub.ro/", "Computer Science");
    /*aloc dinamic stivele pentru navigarea in tab*/
    Tab->backward = (stack *)malloc(sizeof(stack));
    Tab->backward->head = NULL;
    Tab->forward = (stack *)malloc(sizeof(stack));
    Tab->forward->head = NULL;
    /*returnez tabul creat*/
    return Tab;
}

browser *new_tab(browser *b, tablist *head, int *cnt_id)
{
    /*declar si aloc memorie pentru o celula tablist*/
    tablist *newtab = (tablist *)malloc(sizeof(tablist));
    newtab->next = newtab->prev = NULL;

    /*iau intr-un pointer adresa ultimului element din lista*/
    tablist *ptr_curr = head->prev;

    /*initializez tabul cu id-ul egal cu id-ul ultimului tab deschis + 1,
    si apoi il adaug in lista la final*/
    newtab->Tab = init_new_tab(*cnt_id);
    ptr_curr->next = newtab;
    newtab->prev = ptr_curr;
    newtab->next = head;
    head->prev = newtab;

    /*cresc id-ul tabului pentru urmatorul tab care va fi deschis*/
    (*cnt_id) += 1;
    /*actualizez tabul curent*/
    b->currentTab = newtab->Tab;
    /*returnez browserul cu noile modificari*/
    return b;
}

tab *close(tab *currentTab, tablist *head, FILE *out)
{
    /*daca tabul curent este cel cu id ul 0 afisez mesajul de eroare
    si intorc tabul curent cu id 0*/
    if (currentTab->id == 0)
    {
        fprintf(out, "403 Forbidden\n");
        return currentTab;
    }

    /*parcurg lista pana cand gasesc tabul curent*/
    tablist *ptr_curr = head;
    while (ptr_curr->next->Tab->id != currentTab->id && ptr_curr->next != head)
        ptr_curr = ptr_curr->next;

    /*elimin tabul curent*/
    tablist *aux = ptr_curr->next;
    ptr_curr->next = aux->next;
    aux->next->prev = ptr_curr;
    /*tabul curent devine cel care a fost in stanga celui tocmai inchis*/
    currentTab = ptr_curr->Tab;

    /*eliberez memoria tabului inchis din lista*/
    /*eliberez stivele*/
    while (!isEmpty(aux->Tab->backward))
        pop(aux->Tab->backward);
    free(aux->Tab->backward);

    while (!isEmpty(aux->Tab->forward))
        pop(aux->Tab->forward);
    free(aux->Tab->forward);

    /*eliberez pagina web si descrierea acesteia*/
    if (aux->Tab->currentPage)
    {
        if (aux->Tab->currentPage->desc)
            free(aux->Tab->currentPage->desc);
        free(aux->Tab->currentPage);
    }

    /*eliberez celula de tab, respectiv de tablist*/
    free(aux->Tab);
    free(aux);

    /*returnez noul tab curent*/
    return currentTab;
}

tab *open_id(tab *currentTab, tablist *head, int id, FILE *out)
{
    /*parcurg lista pana cand gasesc tabul cu id-ul specificat sau pana se
    termina lista*/
    tablist *ptr_curr = head->next;
    while (ptr_curr->Tab->id != id && ptr_curr != head)
        ptr_curr = ptr_curr->next;

    /*daca am gasit tabul cu id ul specificat, actualizez tabul curent,
    altfel afisez mesajul de eroare*/
    if (ptr_curr->Tab->id == id)
        currentTab = ptr_curr->Tab;
    else
        fprintf(out, "403 Forbidden\n");

    /*returnez tabul curent*/
    return currentTab;
}

tab *next(tab *currentTab, tablist *head)
{
    /*parcurg lista pana cand gasesc tabul cu id-ul specificat*/
    tablist *ptr_curr = head->next;
    while (ptr_curr->Tab->id != currentTab->id)
        ptr_curr = ptr_curr->next;

    /*daca tabul urmator este santinela, sar peste aceasta*/
    if (ptr_curr->next == head)
        ptr_curr = ptr_curr->next;

    /*actualizez tabul curent, astfel incat sa fie urmatorul din lista*/
    currentTab = ptr_curr->next->Tab;
    /*returnez noul tab curent*/
    return currentTab;
}

tab *prev(tab *currentTab, tablist *head)
{
    /*parcurg lista pana cand gasesc tabul cu id-ul specificat*/
    tablist *ptr_curr = head->next;
    while (ptr_curr->Tab->id != currentTab->id)
        ptr_curr = ptr_curr->next;

    /*daca tabul anterior este santinela, sar peste aceasta*/
    if (ptr_curr->prev == head)
        ptr_curr = ptr_curr->prev;

    /*actualizez tabul curent, astfel incat sa fie cel de dinainte din lista*/
    currentTab = ptr_curr->prev->Tab;
    /*returnez noul tab curent*/
    return currentTab;
}

void page_id(tab *currentTab, page **Pages, int nrpag, int id, FILE *out)
{
    /*caut pagina cu id ul specificat in vectorul de pagini*/
    page *newcurrentPage = NULL;
    for (int i = 0; i <= nrpag; i++)
        if (Pages[i]->id == id)
        {
            newcurrentPage = Pages[i];
            break;
        }

    /*daca pagina nu exista afisez mesajul de eroare*/
    if (newcurrentPage == NULL)
    {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    /*daca am gasit-o, pun in stiva backward pagina curenta*/
    push(currentTab->backward, currentTab->currentPage);
    /*sterg continutului stivei forward*/
    while (!isEmpty(currentTab->forward))
        pop(currentTab->forward);

    /*pagina curenta devina noua pagina*/
    currentTab->currentPage = newcurrentPage;
}

void backward(tab *currentTab, FILE *out)
{
    /*daca stiva backward este goala afisez mesajul de eroare*/
    if (isEmpty(currentTab->backward))
    {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    /*imping pagina curenta pe stiva forward*/
    push(currentTab->forward, currentTab->currentPage);
    /*pagina curenta devine ultima pagina pusa pe stiva backward*/
    currentTab->currentPage = currentTab->backward->head->Page;
    /*sterg ultima pagina pusa pe stiva backward*/
    pop(currentTab->backward);
}

void forward(tab *currentTab, FILE *out)
{
    /*daca stiva forward este goala afisez mesajul de eroare*/
    if (isEmpty(currentTab->forward))
    {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    /*imping pagina curenta pe stiva backward*/
    push(currentTab->backward, currentTab->currentPage);
    /*pagina curenta devine ultima pagina pusa pe stiva fowward*/
    currentTab->currentPage = currentTab->forward->head->Page;
    /*sterg ultima pagina pusa pe stiva forward*/
    pop(currentTab->forward);
}

void print(tab *currentTab, tablist *head, FILE *out)
{
    /*parcurg lista pana cand gasesc tabul curent*/
    tablist *ptr_curr = head->next;
    while (ptr_curr->Tab->id != currentTab->id)
        ptr_curr = ptr_curr->next;

    /*afisez id-urile taburile din lista circulara incepand cu tabul curent*/
    while (ptr_curr->next->Tab != currentTab)
    {
        /*daca tabul nu este santinela, ii afisez id ul*/
        if (ptr_curr != head)
            fprintf(out, "%d ", ptr_curr->Tab->id);
        ptr_curr = ptr_curr->next;
    }

    /*pentru ultimul tab, verific ca nu este santinela si ii afisez id ul*/
    if (ptr_curr != head)
        fprintf(out, "%d ", ptr_curr->Tab->id);

    fprintf(out, "\n");
    /*afisez descrierea paginii curente din tabul curent*/
    fprintf(out, "%s\n", currentTab->currentPage->desc);
}

void print_history(tablist *head, int id, FILE *out)
{
    /*parcurg lista pana cand gasesc tabul cu id-ul specificat*/
    tablist *ptr_curr = head->next;
    while (ptr_curr->Tab->id != id && ptr_curr != head)
        ptr_curr = ptr_curr->next;

    /*daca tabul cu id ul specificat nu este deschis afisez mesajul de eroare*/
    if (ptr_curr == head)
    {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    tab *currentTab = ptr_curr->Tab;

    /*intializez un vector de pagini pentru a memora paginile pe care le scot
    din stiva pentru a afisa istoricul*/
    page *stackPages[50];
    int k = 0;

    /*golesc stiva forward si retin paginile*/
    while (!(isEmpty(currentTab->forward)))
    {
        stackPages[k++] = currentTab->forward->head->Page;
        pop(currentTab->forward);
    }

    /*afisez url-urile paginilor care au fost in stiva forward de la prima adaugata*/
    for (int i = k - 1; i >= 0; i--)
        fprintf(out, "%s\n", stackPages[i]->url);

    /*pun paginile inapoi in stiva forward*/
    for (int i = 0; i < k; i++)
        push(currentTab->forward, stackPages[i]);

    /*afisez url-ul paginii curente*/
    fprintf(out, "%s\n", currentTab->currentPage->url);

    k = 0;
    /*golesc stiva backward si retin paginile*/
    while (!(isEmpty(currentTab->backward)))
    {
        stackPages[k++] = currentTab->backward->head->Page;
        pop(currentTab->backward);
    }

    /*afisez url-urile paginilor care au fost in stiva backward de la ultima adaugata*/
    for (int i = 0; i < k; i++)
        fprintf(out, "%s\n", stackPages[i]->url);

    /*pun paginile inapoi in stiva backward*/
    for (int i = k - 1; i >= 0; i--)
        push(currentTab->backward, stackPages[i]);
}

int main(int argc, char *argv[])
{
    /*declar browserul*/
    browser *b;
    int nr_pagini, cnt_id = 0;

    /*aloc dinamic browserul si lista de taburi circulara cu santinela*/
    b = (browser *)malloc(sizeof(browser));
    b->list = (tablist *)malloc(sizeof(tablist));
    b->list->Tab = (tab *)malloc(sizeof(tab));
    b->list->Tab->id = -100;
    b->list->prev = b->list;
    b->list->next = b->list;
    /*deschid tabul cu id 0*/
    b = new_tab(b, b->list, &cnt_id);

    /*deschid fisierele*/
    FILE *in = fopen("tema1.in", "r");
    FILE *out = fopen("tema1.out", "w");

    /*citesc numarul si pagini*/
    fscanf(in, "%d", &nr_pagini);
    fgetc(in);

    /*declar, aloc dinamic vectorul de pagini si initializez pagina cu id ul 0*/
    page **Pages = malloc(sizeof(page *) * (nr_pagini + 1));
    Pages[0] = init_new_page(0, "https://acs.pub.ro/", "Computer Science");

    /*citesc datele paginilor din fisier si le introduc in vectorul de pagini*/
    for (int i = 1; i <= nr_pagini; i++)
    {
        int id;
        char url[50], desc[256];
        fscanf(in, "%d", &id);
        fgetc(in);

        fgets(url, 50, in);
        url[strlen(url) - 1] = '\0';

        fgets(desc, 256, in);
        desc[strlen(desc) - 1] = '\0';

        Pages[i] = init_new_page(id, url, desc);
    }

    /*citesc numarul de operatii*/
    int nr_operatii;
    fscanf(in, "%d", &nr_operatii);
    fgetc(in);

    /*citesc operatiile si apelez functiile corespunzatoare*/
    for (int i = 0; i < nr_operatii; i++)
    {
        char op[20];
        fgets(op, 20, in);
        op[strlen(op) - 1] = '\0';

        if (strcmp(op, "NEW_TAB") == 0)
            b = new_tab(b, b->list, &cnt_id);
        else if (strcmp(op, "CLOSE") == 0)
            b->currentTab = close(b->currentTab, b->list, out);
        else if (strstr(op, "OPEN") != NULL)
        {
            int id = construct_number(op);
            b->currentTab = open_id(b->currentTab, b->list, id, out);
        }
        else if (strcmp(op, "NEXT") == 0)
            b->currentTab = next(b->currentTab, b->list);
        else if (strcmp(op, "PREV") == 0)
            b->currentTab = prev(b->currentTab, b->list);
        else if (strstr(op, "PAGE") != NULL)
        {
            int id = construct_number(op);
            page_id(b->currentTab, Pages, nr_pagini, id, out);
        }
        else if (strcmp(op, "BACKWARD") == 0)
            backward(b->currentTab, out);
        else if (strcmp(op, "FORWARD") == 0)
            forward(b->currentTab, out);
        else if (strcmp(op, "PRINT") == 0)
            print(b->currentTab, b->list, out);
        else if (strstr(op, "HISTORY") != NULL)
        {
            int id = construct_number(op);
            print_history(b->list, id, out);
        }
    }

    /*inchid fisierele*/
    fclose(in);
    fclose(out);

    /*eliberez memoria din lista de taburi*/
    tablist *ptr_curr = b->list->next;
    while (ptr_curr != b->list)
    {
        tablist *to_free = ptr_curr;
        ptr_curr = ptr_curr->next;

        /*elibir memoria din stive*/
        while (!isEmpty(to_free->Tab->backward))
            pop(to_free->Tab->backward);
        free(to_free->Tab->backward);

        while (!isEmpty(to_free->Tab->forward))
            pop(to_free->Tab->forward);
        free(to_free->Tab->forward);

        /*elimin memoria paginii curente si a descrierii*/
        if (to_free->Tab->currentPage)
        {
            if (to_free->Tab->currentPage->desc)
            {
                free(to_free->Tab->currentPage->desc);
                to_free->Tab->currentPage->desc = NULL;
            }
            free(to_free->Tab->currentPage);
            to_free->Tab->currentPage = NULL;
        }

        /*elimin memoria celulei tab, si apoi a celului tablist*/
        free(to_free->Tab);
        free(to_free);
    }

    /*eliberez memoria santinelei si apoi a browserului*/
    free(b->list->Tab);
    free(b->list);
    free(b);

    /*eliberez memoria vectorului de pagini*/
    for (int i = 0; i <= nr_pagini; i++)
    {
        if (Pages[i] != NULL)
        {
            if (Pages[i]->desc != NULL)
                free(Pages[i]->desc);
            free(Pages[i]);
        }
    }
    free(Pages);

    return 0;
}