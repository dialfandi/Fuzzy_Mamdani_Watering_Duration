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

struct shape_of_cos
{
    float A[3];
} shape_of_cos, xa;
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

float flexible_upper_line(float myu, float a, float b, float c, float d)
{
    return (d - a) - (((d - a) - (c - b)) * myu);
}

float set_bottom_line(bool mode, float a, float b)
{
    if (mode)
    {
        return b - a;
    }
    else
    {
        return (b + a) / 2;
    }
}

float one_constrain_shape(unsigned int shape, float myu, float a, float b, float c, float d)
{
    if (shape == 0) // trapezidal_full
    {
        return (((d - a) + (c - b)) / 2) * myu;
    }
    else if (shape == 1) // triangle_full
    {
        return ((c - a) * myu) / 2;
    }
}

float deffuzi_CoS(float myu1, float myu2, float myu3)
{
    if (myu1 == 1 || myu2 == 1 || myu3 == 1)
    {
        shape_of_cos.A[0] = one_constrain_shape(0, myu1, 0, 0, 8, 12);
        shape_of_cos.A[1] = one_constrain_shape(1, myu2, 10, 0, 20, 0);
        shape_of_cos.A[2] = one_constrain_shape(0, myu3, 12, 22, 30, 30);
    }
    else if (myu1 < 1 || myu2 < 1 || myu3 < 1)
    {
        shape_of_cos.A[0] = ((flexible_upper_line(myu1, 0, 0, 8, 12) + set_bottom_line(1, 0, 12)) * myu1) / 2;
        shape_of_cos.A[1] = ((flexible_upper_line(myu2, 10, 0, 0, 20) + set_bottom_line(1, 10, 20)) * myu2) / 2;
        shape_of_cos.A[2] = ((flexible_upper_line(myu3, 18, 22, 30, 30) + set_bottom_line(1, 18, 30)) * myu3) / 2;
    }
    xa.A[0] = set_bottom_line(0, 12, 0);
    xa.A[1] = set_bottom_line(0, 10, 20);
    xa.A[2] = set_bottom_line(0, 18, 30);
    return (((shape_of_cos.A[0] * xa.A[0]) + (shape_of_cos.A[1] * xa.A[1]) + (shape_of_cos.A[2] * xa.A[2])) / (shape_of_cos.A[0] + shape_of_cos.A[1] + shape_of_cos.A[2]));
}

void inference(int max_count)
{
    // Set to 0 value
    duration.low_result = 0;
    duration.med_result = 0;
    duration.hig_result = 0;
    // Created Rule Base "Minimum Selection"
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
        // printf("Durasi Penyiraman (duration) 'Lama': %.2f\n", duration.hig_result);
        printf("==========>LOOP KE-%d<==========\n", i);
        printf("duration LOW: %.2f\n", duration.low[i]);
        printf("duration MEDIUM: %.2f\n", duration.medium[i]);
        printf("duration HIGH: %.2f\n", duration.high[i]);
        // seleksi nilai terbesar
        if (i >= 0 && i < count_low)
        {
            if (duration.low[i] > duration.low_result)
            {
                duration.low_result = duration.low[i];
            }
            else
            {
                // printf("CHECK3\n");
            }
        }
        if (i >= 0 && i < count_med)
        {
            if (duration.medium[i] > duration.med_result)
            {
                duration.med_result = duration.medium[i];
            }
            else
            {
                // printf("CHECK2\n");
            }
        }
        if (i >= 0 && i < count_hig)
        {
            if (duration.high[i] > duration.hig_result)
            {
                duration.hig_result = duration.high[i];
                // printf("Durasi Penyiraman (duration) 'Lama': %.2f\n", duration.hig_result);
            }
            else
            {
                // printf("CHECK3\n");
            }
        }
    }
    printf("============> INFERENCE RESULT <==========\n");
    printf("Durasi Penyiraman (duration) 'Singkat': %.2f\n", duration.low_result);
    printf("Durasi Penyiraman (duration) 'Sedang': %.2f\n", duration.med_result);
    printf("Durasi Penyiraman (duration) 'Lama': %.2f\n", duration.hig_result);
}

float deffuzi_WG(float low_minmax, float med_minmax, float high_minmax, float low_set, float medium_set, float high_set)
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
        float result_of_WG = deffuzi_WG(duration.low_result, duration.med_result, duration.hig_result, 8, 15, 22);
        float result_of_CoS = deffuzi_CoS(duration.low_result, duration.med_result, duration.hig_result);
        printf("============> RESULT OF DEFFUZIFICATION 1<=============\n");
        //printf("shape1: %.2f || %.2f || %.2f || %.2f\n", shape_of_cos.A[0], xa.A[0], flexible_upper_line(duration.low_result, 0, 0, 8, 12), set_bottom_line(1, 0, 12) * duration.low_result / 2);
        //printf("shape2: %.2f || %.2f\n", shape_of_cos.A[1], xa.A[1]);
        //printf("shape3: %.2f || %.2f\n", shape_of_cos.A[2], xa.A[2]);
        printf("Result WeG: %.2f menit\n", result_of_WG);
        printf("Result CoS: %.2f menit\n", result_of_CoS);

        printf("Continue The Fuzzy System? ");
        scanf("%d", &input_count);
    } while (input_count != 0);
    return 0;
}

// T/H	D	H	P
// K	SD	L	L
// N	S	SD	S
// L	S	SD	SD
