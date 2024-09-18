#include <stdio.h>
#include <stdbool.h>
// definisi struct untuk menyimpan keanggotaan suhu dan kelembaban
struct member_input
{
    // member temperature: 0(Dingin), 1(Hangat), 2(Panas)
    // member humidity: 0(Kering), 1(Normal), 2(Lembab)
    float member[3];
    float crisp_input;
} temperature, humidity;

int input_count = 0;
int max_array_count = 4;
int count_low = 3;
int count_med = 4;
int count_hig = 2;

struct member_output
{
    // Member duration: 0(Singkat), 1(Sidang), 2(Lama)
    float low[3];
    float medium[4];
    float high[2];
    float low_result;
    float med_result;
    float hig_result;
} duration;

// fungsi menghitung nilai max
float max(float a, float b) { return a > b ? a : b; }

// fungsi menghitung nilai min
float min(float a, float b) { return a < b ? a : b; }

// fungsi menghitung keanggotaan grafik naik dan turun fuzzy : 1 (naik) & 0 (turun)
float up_down(bool updown_formulation, float x, float a, float b)
{
    if (updown_formulation)
    {
        if (x <= a)
        {
            return 1;
        }
        else if (x > a && x < b)
        {
            return (b - x) / (b - a);
        }
        else if (x >= b)
        {
            return 0;
        }
    }
    else
    {
        if (x <= a)
        {
            return 0;
        }
        else if (x > a && x < b)
        {
            return (x - a) / (b - a);
        }
        else if (x >= b)
        {
            return 1;
        }
    }
};

// fungsi menghitung keanggotaan fuzzy dalam bentuk grafik triangular
float triangular(float x, float a, float b, float c)
{
    float hasil;
    if (x <= a)
    {
        return 0;
    }
    else if (x >= a && x <= b)
    {
        return (x - a) / (b - a);
    }
    else if (x >= b && x <= c)
    {
        return (c - x) / (c - b);
    }
    else if (x >= c)
    {
        return 0;
    }
};

// fungsi menghitung keanggotaan fuzzy dalam bentuk grafik trapezoidal
float trapezoidal(float x, float a, float b, float c, float d)
{
    if (x <= a)
    {
        return 0;
    }
    else if (x >= a && x <= b)
    {
        return (x - a) / (b - a);
    }
    else if (x >= b && x <= c)
    {
        return 1;
    }
    else if (x >= c && x <= d)
    {
        return (d - x) / (d - c);
    }
    else if (x >= d)
    {
        return 0;
    }
};

void fuzzifikasi(bool member, float input)
{
    if (member)
    {
        if (input >= 0 && input <= 50)
        {
            temperature.member[0] = up_down(1, input, 15.0, 25.0);
            temperature.member[1] = trapezoidal(input, 15.0, 25.0, 30.0, 40.0);
            temperature.member[2] = up_down(0, input, 30.0, 40.0);
        }
        else
        {
            printf("INPUT AGAIN\n");
            input_count = 0;
        }
    }
    else
    {
        if (input >= 0 && input <= 100)
        {
            humidity.member[0] = up_down(1, input, 30.0, 50.0);
            humidity.member[1] = triangular(input, 30.0, 55.0, 80.0);
            humidity.member[2] = up_down(0, input, 60.0, 80.0);
        }
        else
        {
            printf("INPUT AGAIN\n");
            input_count = 0;
        }
    }
}

void inference(int max_count)
{
    //Set to 0 value
    duration.low_result = 0;
    duration.med_result = 0;
    duration.hig_result = 0;
    //Created Rule Base "Minimum Selection"
    duration.low[0] = min(temperature.member[0], humidity.member[1]);
    duration.low[1] = min(temperature.member[2], humidity.member[1]);
    duration.low[2] = min(temperature.member[0], humidity.member[2]);
    // //ID = 3
    duration.medium[0] = min(temperature.member[0], humidity.member[0]);
    duration.medium[1] = min(temperature.member[1], humidity.member[1]);
    duration.medium[2] = min(temperature.member[1], humidity.member[2]);
    duration.medium[3] = min(temperature.member[2], humidity.member[2]);
    // //ID = 4
    duration.high[0] = min(temperature.member[1], humidity.member[0]);
    duration.high[1] = min(temperature.member[2], humidity.member[0]);

    printf("============> DEGREE OF MEMBERSHIP FUZZY MINIMA <=============\n");
    for (int i = 0; i < max_count; i++)
    {
        //printf("Durasi Penyiraman (duration) 'Lama': %.2f\n", duration.hig_result);
        printf("==========>LOOP KE-%d<==========\n", i);
        printf("duration LOW: %.2f\n", duration.low[i]);
        printf("duration MEDIUM: %.2f\n", duration.medium[i]);
        printf("duration HIGH: %.2f\n", duration.high[i]);
        //seleksi nilai terbesar
        if (i >= 0 && i < count_low)
        {
            if (duration.low[i] > duration.low_result)
            {
                duration.low_result = duration.low[i];
            }
            else{
                //printf("CHECK3\n");
            }
        }
        if (i >= 0 && i < count_med)
        {
            if (duration.medium[i] > duration.med_result)
            {
                duration.med_result = duration.medium[i];
            }
            else{
                //printf("CHECK2\n");
            }
        }
        if (i >= 0 && i < count_hig)
        {
            if (duration.high[i] > duration.hig_result)
            {
                duration.hig_result = duration.high[i];
                //printf("Durasi Penyiraman (duration) 'Lama': %.2f\n", duration.hig_result);
            }
            else{
                //printf("CHECK3\n");
            }
        }
    }
    printf("============> INFERENCE RESULT <==========\n");
    printf("Durasi Penyiraman (duration) 'Singkat': %.2f\n", duration.low_result);
    printf("Durasi Penyiraman (duration) 'Sedang': %.2f\n", duration.med_result);
    printf("Durasi Penyiraman (duration) 'Lama': %.2f\n", duration.hig_result);
}

float deffuzi_CoG(float low_minmax, float med_minmax, float high_minmax, float low_set, float medium_set, float high_set)
{
    return ((low_minmax * low_set) + (med_minmax * medium_set) + (high_minmax * high_set)) / (low_minmax + med_minmax + high_minmax);
}

int main()
{
    int count = 0;
    do
    {
        printf("Give a input value of temperature : ");
        scanf("%f", &temperature.crisp_input);
        printf("Give a input value of humidity : ");
        scanf("%f", &humidity.crisp_input);

        fuzzifikasi(1, temperature.crisp_input);
        fuzzifikasi(0, humidity.crisp_input);

        printf("============> DEGREE OF MEMBERSHIP FUZZY TEMPERATURE <===========\n");
        printf("Dingin  : %.2f\n", temperature.member[0]);
        printf("Hangat  : %.2f\n", temperature.member[1]);
        printf("Panas   : %.2f\n", temperature.member[2]);

        printf("============> DEGREE OF MEMBERSHIP FUZZY HUMIDITY <=============\n");
        printf("Kering  : %.2f\n", humidity.member[0]);
        printf("Normal  : %.2f\n", humidity.member[1]);
        printf("Lembab  : %.2f\n", humidity.member[2]);

        inference(max_array_count);
        float result_of_CoG = deffuzi_CoG(duration.low_result, duration.med_result, duration.hig_result, 10, 20, 30);
        printf("============> RESULT OF DEFFUZIFICATION <=============\n");
        printf("Result: %.2f menit\n", result_of_CoG);

        printf("Continue The Fuzzy System? ");
        scanf("%d", &input_count);
    } while (input_count != 0);
    return 0;
}

// T/H	D	H	P
// K	SD	L	L
// N	S	SD	S
// L	S	SD	SD
