// version 1.4 -- disabled the sorting and added indexes (function 9 not working!!!)
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h> // case 8

FILE *fp, *fp_temp, *fp_Index, *fp_temp_Index;
char *filename = "C:\\Programming\\ap3\\students.txt";
char *filename_temp = "C:\\Programming\\ap3\\students_temp.txt";
char *filename_Index = "C:\\Programming\\ap3\\Index.txt";
char *filename_temp_Index = "C:\\Programming\\ap3\\Index_temp.txt";
char student_removal[15] = "**************"; // for case 7

struct Students
{
    char RegistrationNumber[40];
    char Last_Name[40];
    char First_Name[40];
    float Physics_I;
    float Data_bases_and_files;
    float Mathematical_Analysis_II;
    float Computer_Architecture_I;
    float Programming_II;
    float GPA; // for case 9
};

struct Index
{
    char RegistrationNumber[40];
    int pos; // position
};

struct Students S;
struct Index I;

void menu();
void creation_or_expansion();
void display_all();
void search();
void swhpatc(); // students who have passed all their courses
void swhfadb(); // students who have failed at data bases
int c_search(void);
void correct();     // corrects a student's info
void remove_st();   // removes a student's info
void remove_perm(); // removes a student's info permanently and updates the students.txt file with the non deleted students
void sort();        // sorts the students in ascending order based on their registration number
void gpa();
void flush_deleted_students();

int main()
{

    menu();
}

void menu()
{
    int user_choice;
    do
    {
        printf("\n Welcome to the menu. Type one number (1-10) to perform a specific task as shown below: \n");
        printf("1. Creation/Expansion \n2. Displays all students (also displaying their GPAs)\n");
        printf("3. Searches for a student \n4. Displays all the students that have passed all their courses \n");
        printf("5. Displays all the students that have failed at data bases \n6. Corrects a student's info \n");
        printf("7. Removes a student's info \n8. Removes a student's info permanenty\n");
        printf("9. Sorts all the students based on their GPA in descending order (disabled). \n");
        printf("10. Flushes the deleted students.\n99. Exits the program. \n");
        scanf("%d", &user_choice);
    } while ((user_choice < 1 || user_choice > 10) && user_choice != 99);

    switch (user_choice)
    {

    case 1:
        creation_or_expansion();
        break;

    case 2:
        display_all();
        break;

    case 3:
        search();
        break;

    case 4:
        swhpatc();
        break;

    case 5:
        swhfadb();
        break;

    case 6:
        correct();
        break;

    case 7:
        remove_st();
        break;

    case 8:
        remove_perm();
        break;

    case 9:
        //sort();
        printf("\n Function disabled! \n");
        break;
    case 10:
        flush_deleted_students(); // my case. empties both files (students.txt, Index.txt) from the deleted students
        break;
    case 99:
        exit(1);
        break;
    }
}

void creation_or_expansion() // case 1
{
    int N;

    fp = fopen(filename, "rb+");
    fp_Index = fopen(filename_Index, "rb+");

    if (!fp)
    {
        printf("\n Creating a new file called: students.txt\n");
        printf("\n Creating a new file called: Index.txt\n");
        fp = fopen(filename, "wb");
        fp_Index = fopen(filename_Index, "wb");
        exit(1);
    }

    else
    {
        printf("\n Expanding an existent file. \n");
        fseek(fp, 0, SEEK_END);
        fseek(fp_Index, 0, SEEK_END);
        N = ftell(fp_Index) / sizeof(struct Index);
    }
    // adding new students to the file
    int continue_adding; // 0 to stop adding students, 1 to continue adding students

    do
    {
        printf("\n Enter: registration number, last name, first name, physics I mark, data bases and files mark, mathematical analysis II mark, Computer architecture I mark and programming II mark: \n");
        scanf("%s%s%s%f%f%f%f%f", &S.RegistrationNumber, &S.Last_Name, &S.First_Name, &S.Physics_I, &S.Data_bases_and_files, &S.Mathematical_Analysis_II, &S.Computer_Architecture_I, &S.Programming_II);
        fwrite(&S, sizeof(struct Students), 1, fp);
        strcpy(I.RegistrationNumber, S.RegistrationNumber);
        I.pos = N;
        fwrite(&I, sizeof(struct Index), 1, fp_Index);
        N++;
        printf("\n Do you wish to continue adding students (0 for no, 1 for yes)? \n");
        scanf("%d", &continue_adding);
    } while (continue_adding == 1);

    fclose(fp);
    fclose(fp_Index);
}

void display_all() // case 2
{

    fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    printf("\n All the students will be displayed alongside their GPA. \n");
    float GPA;               // the GPA of each student perspectively
    int student_counter = 0; // changes value when a non-deleted student is read from the file. If all the students are deleted then the program displays to the user that all the students are deleted (if student_counter == 0).

    while (!feof(fp))
    {
        fread(&S, sizeof(struct Students), 1, fp);
        if (!feof(fp) && strcmp(S.RegistrationNumber, student_removal) != 0) // displays the students' info properly 
        {
            GPA = (S.Physics_I + S.Data_bases_and_files + S.Mathematical_Analysis_II + S.Computer_Architecture_I + S.Programming_II) / 5;
            printf("\n **************************** \n Registration Number: %s \n Last Name: %s \n First Name: %s \n Physics I Mark: %.2f \n Data bases and files Mark: %.2f \n Mathematical Analysis II Mark: %.2f \n Computer Architecture Mark: %.2f \n Programming II Mark: %.2f \n GPA: %.2f \n **************************** \n", S.RegistrationNumber, S.Last_Name, S.First_Name, S.Physics_I, S.Data_bases_and_files, S.Mathematical_Analysis_II, S.Computer_Architecture_I, S.Programming_II, GPA);
            student_counter++;
        }
    };

    if (student_counter == 0)
        printf("\n All the students are deleted! \n");

    fclose(fp);
}

void search() // case 3
{
    fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    fp_Index = fopen(filename_Index, "rb");

    // searches the database using the user-input registration number
    char reg_num[40];
    printf("\n Enter a registration number: \n");
    scanf("%s", &reg_num);
    bool flag = false; //gets set to true if the registration number has been found
    while (!feof(fp_Index))
    {
        fread(&I, sizeof(struct Index), 1, fp_Index);
        if (strcmp(I.RegistrationNumber, student_removal) != 0 && strcmp(I.RegistrationNumber, reg_num) == 0)
        {
            flag = true;
            break;
        }
    }
    if (flag == false)
        printf("\n The student with the registration number \'%s\' was not found in our data base.", reg_num);
    else
    {
        fseek(fp, I.pos * sizeof(struct Students), SEEK_SET);
        fread(&S, sizeof(struct Students), 1, fp);
        printf("\n Displaying info for the student with the registration number: \'%s\' \n", reg_num);
        printf("\n **************************** \n Registration Number: %s \n Last Name: %s \n First Name: %s \n Physics I Mark: %.2f \n Data bases and files Mark: %.2f \n Mathematical Analysis II Mark: %.2f \n Computer Architecture Mark: %.2f \n Programming II Mark: %.2f \n **************************** \n", S.RegistrationNumber, S.Last_Name, S.First_Name, S.Physics_I, S.Data_bases_and_files, S.Mathematical_Analysis_II, S.Computer_Architecture_I, S.Programming_II);
    }

    fclose(fp);
    fclose(fp_Index);
}

void swhpatc() // case 4
{
    fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    printf("\n All the students who have passed all their courses will be displayed below: \n");

    int courses_counter = 0; // counts the students who have passed all their courses (when 0, a specific message is displayed)
    while (!feof(fp))
    {
        fread(&S, sizeof(struct Students), 1, fp);
        if (strcmp(S.RegistrationNumber, student_removal) != 0)
            if (S.Computer_Architecture_I >= 5 && S.Data_bases_and_files >= 5 && S.Mathematical_Analysis_II >= 5 && S.Physics_I >= 5 && S.Programming_II >= 5)
            {
                printf("\n **************************** \n %s \n %s \n %s \n %.2f \n %.2f \n %.2f \n %.2f \n %.2f \n **************************** \n", S.RegistrationNumber, S.Last_Name, S.First_Name, S.Physics_I, S.Data_bases_and_files, S.Mathematical_Analysis_II, S.Computer_Architecture_I, S.Programming_II);
                courses_counter++;
            }
    }

    if (courses_counter == 0)
        printf("\n All the students have failed to pass all their courses! \n");

    fclose(fp);
}

void swhfadb() // case 5
{
    fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    printf("\n All the students who failed at data bases will be displayed. \n");

    while (!feof(fp))
    {
        fread(&S, sizeof(struct Students), 1, fp);
        if (strcmp(S.RegistrationNumber, student_removal) != 0)
            if (S.Data_bases_and_files < 5)
                printf("\n **************************** \n Registration Number: %s \n Last Name: %s \n First Name: %s \n Physics I Mark: %.2f \n Data bases and files Mark: %.2f \n Mathematical Analysis II Mark: %.2f \n Computer Architecture Mark: %.2f \n Programming II Mark: %.2f \n **************************** \n", S.RegistrationNumber, S.Last_Name, S.First_Name, S.Physics_I, S.Data_bases_and_files, S.Mathematical_Analysis_II, S.Computer_Architecture_I, S.Programming_II);
    }

    fclose(fp);
}

void correct() // case 6, #include <string.h>
{
    fp = fopen(filename, "rb+");
    fp_Index = fopen(filename_Index, "rb+");
    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    bool flag = 0;  // gets set to true when the student is found
    char r_num[30]; // registration number entered by the user
    printf("\n Enter the registration number of the student: \n");
    scanf("%s", &r_num);
    while (!feof(fp_Index) && (flag == false))
    {
        fread(&I, sizeof(struct Index), 1, fp_Index);
        if (!feof(fp_Index) && strcmp(I.RegistrationNumber, r_num) == 0)
        {
            flag = true;
            fseek(fp, I.pos * sizeof(struct Students), SEEK_SET);
            fseek(fp, -sizeof(struct Students), SEEK_CUR);

            printf("\n Enter: registration number, last name, first name, physics I mark, data bases and files mark, mathematical analysis II mark, Computer architecture I mark and programming II mark: \n");
            scanf("%s%s%s%f%f%f%f%f", &S.RegistrationNumber, &S.Last_Name, &S.First_Name, &S.Physics_I, &S.Data_bases_and_files, &S.Mathematical_Analysis_II, &S.Computer_Architecture_I, &S.Programming_II);

            // students.txt
            fwrite(&S, sizeof(struct Students), 1, fp);

            // Index.txt
            fseek(fp_Index, -sizeof(struct Index), SEEK_CUR);
            strcpy(I.RegistrationNumber, S.RegistrationNumber);
            fwrite(&I, sizeof(struct Index), 1, fp_Index);

            printf("\n The student's info has been successfully updated! \n");
        }
    }
    if (flag == false)
        printf("\n The student with the registration number \'%s\' was not found in our data base.", r_num);

    fclose(fp);
    fclose(fp_Index);
}

void remove_st() // case 7
{
    fp = fopen(filename, "rb+");
    fp_Index = fopen(filename_Index, "rb+");

    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    // searches the database using the user-input registration number
    char reg_num[40];
    printf("\n Enter a registration number: \n");
    scanf("%s", &reg_num);
    bool flag = false; //gets set to true if the registration number has been found
    while (!feof(fp_Index) && flag == false)
    {
        fread(&I, sizeof(struct Index), 1, fp_Index);
        if (strcmp(I.RegistrationNumber, reg_num) == 0)
        {
            flag = true;
        }
    }

    if (flag == false)
    {
        printf("\n The student with the registration number \'%s\' was not found in our data base.", reg_num);
    }
    else
    {
        fseek(fp, I.pos * sizeof(struct Students), SEEK_SET);
        fread(&S, sizeof(struct Students), 1, fp);
        printf("\n Displaying info for the student with the registration number: \'%s\' \n", reg_num);
        printf("\n **************************** \n Registration Number: %s \n Last Name: %s \n First Name: %s \n Physics I Mark: %.2f \n Data bases and files Mark: %.2f \n Mathematical Analysis II Mark: %.2f \n Computer Architecture Mark: %.2f \n Programming II Mark: %.2f \n **************************** \n", S.RegistrationNumber, S.Last_Name, S.First_Name, S.Physics_I, S.Data_bases_and_files, S.Mathematical_Analysis_II, S.Computer_Architecture_I, S.Programming_II);

        int user_choice; // the user's choice whether or not they want to delete a student's data
        do
        {
            printf("\n Do you wish to delete this student's data (0 for no, 1 for yes)? \n");
            scanf("%d", &user_choice);
        } while (user_choice < 0 || user_choice > 1);

        if (user_choice == 1)
        {
            printf("\n Proceeding to delete student's data... \n");
            fseek(fp, -sizeof(struct Students), SEEK_CUR);
            strcpy(S.RegistrationNumber, student_removal);
            fwrite(&S, sizeof(struct Students), 1, fp);
            fseek(fp_Index, -sizeof(struct Index), SEEK_CUR);
            strcpy(I.RegistrationNumber, student_removal);
            fwrite(&I, sizeof(struct Index), 1, fp_Index);
            printf("\n The student's data has been successfully deleted! \n");
        }
        else
        {
            printf("\n Exiting the program... \n");
        }
    }

    fclose(fp);
    fclose(fp_Index);
}

void remove_perm() // case 8
{

    fp = fopen(filename, "rb+");
    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    fp_Index = fopen(filename_Index, "rb+");

    // searches the database using the user-input registration number
    char reg_num[40];
    printf("\n Enter a registration number: \n");
    scanf("%s", &reg_num);
    bool flag = false; // registration number not found
    while (!feof(fp_Index))
    {
        fread(&I, sizeof(struct Index), 1, fp_Index);
        if (strcmp(I.RegistrationNumber, student_removal) != 0 && strcmp(I.RegistrationNumber, reg_num) == 0)
        {
            flag = true;
            break;
        }
    }
    if (flag == false)
        printf("\n The student with the registration number \'%s\' was not found in our data base.", reg_num);
    else
    {
        fseek(fp, I.pos * sizeof(struct Students), SEEK_SET);
        fread(&S, sizeof(struct Students), 1, fp);
        printf("\n Displaying info for the student with the registration number: \'%s\' \n", reg_num);
        printf("\n **************************** \n Registration Number: %s \n Last Name: %s \n First Name: %s \n Physics I Mark: %.2f \n Data bases and files Mark: %.2f \n Mathematical Analysis II Mark: %.2f \n Computer Architecture Mark: %.2f \n Programming II Mark: %.2f \n **************************** \n", S.RegistrationNumber, S.Last_Name, S.First_Name, S.Physics_I, S.Data_bases_and_files, S.Mathematical_Analysis_II, S.Computer_Architecture_I, S.Programming_II);

        int user_choice;
        do
        {
            printf("\n Are you sure that you want to delete this student's data (0 for no, 1 for yes)? \n");
            scanf("%d", &user_choice);
        } while (user_choice < 0 || user_choice > 1);

        if (user_choice == 0)
        {
            printf("\n Exiting the program... \n");
            exit(1);
        }
        else
        {

            // the student's registration number will be replaced with asterisks

            //students.txt
            fseek(fp, -sizeof(struct Students), SEEK_CUR);
            strcpy(S.RegistrationNumber, student_removal);
            fwrite(&S, sizeof(struct Students), 1, fp);

            // Index.txt
            fseek(fp_Index, -sizeof(struct Index), SEEK_CUR);
            strcpy(I.RegistrationNumber, student_removal);
            fwrite(&I, sizeof(struct Index), 1, fp_Index);

            // copying the non deleted students to the temp file

            // students_temp.txt
            fseek(fp, 0, SEEK_SET);
            fp_temp = fopen(filename_temp, "wb+");

            // Index_temp.txt
            fseek(fp_Index, 0, SEEK_SET);
            fp_temp_Index = fopen(filename_temp_Index, "wb+");

            if (!fp_temp)
            {
                printf("\n An error occurred:");
                printf("", errno);
                exit(1);
            }

            if (!fp_temp_Index)
            {
                printf("\n An error occurred:");
                printf("", errno);
                exit(1);
            }

            // students_temp.txt
            while (!feof(fp))
            {
                fread(&S, sizeof(struct Students), 1, fp);
                if (!feof(fp) && strcmp(S.RegistrationNumber, student_removal) != 0)
                    fwrite(&S, sizeof(struct Students), 1, fp_temp);
            }

            // Index_temp.txt
            while (!feof(fp_Index))
            {
                fread(&I, sizeof(struct Index), 1, fp_Index);
                if (!feof(fp_Index) && strcmp(I.RegistrationNumber, student_removal) != 0)
                    fwrite(&I, sizeof(struct Index), 1, fp_temp_Index);
            }
            printf("\n The non deleted students have been copied to the temporary file successfully. \n");

            // deleting the students.txt, Index.txt files

            // deleting students.txt
            fclose(fp);
            system("del C:\\Programming\\ap3\\students.txt");
            printf("\n The \'students.txt\' file has been deleted successfully. \n");

            // deleting Index.txt
            fclose(fp_Index);
            system("del C:\\Programming\\ap3\\Index.txt");
            printf("\n The \'Index.txt\' file has been deleted successfully. \n");

            // creating a new students.txt file and then copying the contents of the students_temp.txt to the students.txt file
            fp = fopen(filename, "wb");

            if (!fp)
            {
                printf("\n An error occurred:");
                printf("", errno);
                exit(1);
            }

            fseek(fp_temp, 0, SEEK_SET);
            while (!feof(fp_temp))
            {
                fread(&S, sizeof(struct Students), 1, fp_temp);
                if (!feof(fp_temp))
                {
                    fwrite(&S, sizeof(struct Students), 1, fp);
                }
            }

            // deleting the students_temp.txt file
            fclose(fp_temp);
            system("del C:\\Programming\\ap3\\students_temp.txt");

            // closing the students.txt file
            fclose(fp);

            // creating a new Index.txt file and then copying the contents of the Index_temp.txt to the Index.txt file
            fp_Index = fopen(filename_Index, "wb");

            if (!fp_Index)
            {
                printf("\n An error occurred:");
                printf("", errno);
                exit(1);
            }

            fseek(fp_temp_Index, 0, SEEK_SET);
            while (!feof(fp_temp_Index))
            {
                fread(&I, sizeof(struct Index), 1, fp_temp_Index);
                if (!feof(fp_temp_Index))
                {
                    fwrite(&I, sizeof(struct Index), 1, fp_Index);
                }
            }

            // deleting the Index_temp.txt file
            fclose(fp_temp_Index);
            system("del C:\\Programming\\ap3\\Index_temp.txt");

            // closing the Index.txt file
            fclose(fp_Index);

            printf("\n The data base has been updated successfully (with the non deleted students)! \n");
        }
    }
};

void gpa() // function that calculates the gpa of each student
{

    while (!feof(fp))
    {
        fread(&S, sizeof(struct Students), 1, fp);
        if (!feof(fp))
        {
            S.GPA = (S.Physics_I + S.Data_bases_and_files + S.Mathematical_Analysis_II + S.Computer_Architecture_I + S.Programming_II) / 5;
        }
    }
};
/*
void sort() // case 9
{

    fp = fopen(filename, "rb+");

    if (!fp)
    {
        printf("\n Creating a new file called: students.txt\n");
        fp = fopen(filename, "wb");
        exit(1);
    }

    gpa();

    struct Students *GPA_student, temp;

    // (a) calculates the size of the file and then, (b) allocates the neccessary space to the GPA_student dynamic structure

    // (a)

    fseek(fp, 0, SEEK_END);
    int sizeoffile = ftell(fp);
    int amountofstudents = sizeoffile / sizeof(struct Students);

    // (b)

    GPA_student = malloc(amountofstudents * sizeof(struct Students));
    if (GPA_student == NULL)
    {
        printf("\n File too big to transfer to an array. \n");
        exit(1);
    }

    fseek(fp, 0, SEEK_SET);
    fread(&GPA_student[0], sizeof(struct Students), amountofstudents, fp);
    fclose(fp);

    // sorting the students in ascending order based on their registration number
    int i, j, min;

    for (i = 0; i < amountofstudents - 1; i++)
    {
        min = i;
        for (j = i + 1; j < amountofstudents; j++)
        {
            if (GPA_student[j].GPA > GPA_student[min].GPA)
            {
                min = j;
            }
        }
        temp = GPA_student[j];
        GPA_student[j] = GPA_student[min];
        GPA_student[min] = temp;
    }

    fseek(fp, 0, SEEK_SET); // to be deleted
    fwrite(&GPA_student[0], sizeof(struct Students), amountofstudents, fp);

    printf("\n The sorting function has been successfully completed! \n");

    fclose(fp);
}; */

void flush_deleted_students() //case 10
{
    fp = fopen(filename, "rb");
    fp_temp = fopen(filename_temp, "wb+");

    fp_Index = fopen(filename_Index, "rb");
    fp_temp_Index = fopen(filename_temp_Index, "wb+");

    if (!fp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }
    if (!fp_Index)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }
    if (!fp_temp)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }
    if (!fp_temp_Index)
    {
        perror("\n An error occurred: ");
        printf("", errno);
        exit(1);
    }

    // copying the non deleted students to the students_temp.txt, Index_temp.txt files

    // students.txt -> (=to) students_temp.txt
    fseek(fp, 0, SEEK_SET);
    while (!feof(fp))
    {
        fread(&S, sizeof(struct Students), 1, fp);
        if (!feof(fp) && strcmp(S.RegistrationNumber, student_removal) != 0)
        {
            fwrite(&S, sizeof(struct Students), 1, fp_temp);
        }
    }

    // Index.txt -> Index_temp.txt
    fseek(fp_Index, 0, SEEK_SET);
    while (!feof(fp_Index))
    {
        fread(&I, sizeof(struct Index), 1, fp_Index);
        if (!feof(fp_Index) && strcmp(I.RegistrationNumber, student_removal) != 0)
        {
            fwrite(&I, sizeof(struct Index), 1, fp_temp_Index);
        }
    }

    printf("\n The non deleted students have been copied to the temporary files successfully! \n");
    printf("\n Proceeding to close and create the students.txt, Index.txt files again and then copy to them the non deleted students. \n");

    // closing both files
    fclose(fp);
    fclose(fp_Index);

    // deleting both files
    system("del C:\\Programming\\ap3\\students.txt");
    system("del C:\\Programming\\ap3\\Index.txt");
    printf("\n Files deleted successfully! \n");

    // recreating them
    fp = fopen(filename, "wb");
    fp_Index = fopen(filename_Index, "wb");

    // copying the students to them

    // students_temp.txt
    fseek(fp_temp, 0, SEEK_SET);
    while (!feof(fp_temp))
    {
        fread(&S, sizeof(struct Students), 1, fp_temp);
        if (!feof(fp_temp))
            fwrite(&S, sizeof(struct Students), 1, fp);
    }

    //Index_temp.txt
    fseek(fp_temp_Index, 0, SEEK_SET);
    while (!feof(fp_temp_Index))
    {
        fread(&I, sizeof(struct Index), 1, fp_temp_Index);
        if (!feof(fp_temp_Index))
            fwrite(&I, sizeof(struct Index), 1, fp_Index);
    }

    printf("\n Both files have been copied successfully! \n");

    // deleting the temporary files
    fclose(fp_temp);
    fclose(fp_temp_Index);
    system("del C:\\Programming\\ap3\\students_temp.txt");
    system("del C:\\Programming\\ap3\\Index_temp.txt");

    printf("\n The temporary files have been deleted successfully! \n");
    printf("\n The non deleted students have been copied successfully! \n");

    //closing the files
    fclose(fp);
    fclose(fp_Index);
};
