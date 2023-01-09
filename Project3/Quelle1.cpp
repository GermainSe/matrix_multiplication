#define HAVE_STRUCT_TIMESPEC
#include <iostream>
#include <intrin.h>
#include <stdio.h>

#include <stdlib.h>
#include <omp.h>

#include <windows.h>

int const size = 4096;
int tz_512 = 64;
int tz_256 = 64;
int tz_128 = 64;

float aa[size][size], bb[size][size], cc[size][size], cc1[size][size], cc2[size][size], cc6[size][size], cc4[size][size], cc5[size][size];

int const T = 12;
int begin[T];
int end[T];






float randomFloat()
{
	return (float)(rand()) / (float)(rand());
}

void create_random_matrix() 
{
	

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			aa[i][j] = randomFloat();
		}
	}
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			bb[i][j] = randomFloat();
		}
	}
	std::cout << size << std::endl;
	
	//std::cout << A[0][0] << std::endl;

}




void matrix_multiplication()
{
#pragma omp parallel for

	for (int k = 0; k < size; k++) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				cc4[k][i] += aa[k][j] * bb[j][i];
				
			}
			
		}
	}
}
void matrix_multiplication_reihenweise()
{
#pragma omp parallel for
	for (int k = 0; k < size; k++) {
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				cc5[k][i] += aa[k][j] * bb[j][i];
				

			}

		}
	}
}



/*void clear_matrix(float** c1, int size) {
	for (int i = 0; i < size; i++) {
		std::fill_n(c1[i], size, 0);
	}

}	*/



    
void test(float t1[size][size], float t2[size][size]) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (t1[i][j] == t2[i][j] || t1[i][j] != t1[i][j] && t2[i][j]) {
				continue;
			}
			else {
				if (t1[i][j] > t2[i][j] && t1[i][j] / t2[i][j] <= 1.05) {
					continue;
					
				}
				else if (t2[i][j] > t1[i][j] && t2[i][j] / t1[i][j] <= 1.05) {
					continue;
				}
				else {
					std::cout << t2[i][j] << " Ungleich " << t1[i][j] << " " << i << " " << j << std::endl;
					exit(-1);
				}
				
			}
		}
	}
	std::cout << "test ok" << std::endl;
}	

void test_points(float t1[size][size], float t2[size][size]) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (t1[i][j] == t2[i][j] || t1[i][j] != t1[i][j] && t2[i][j]) {
				continue;
			}
			else {
				if (t1[i][j] > t2[i][j] && t1[i][j] / t2[i][j] <= 1.05) {
					continue;

				}
				else if (t2[i][j] > t1[i][j] && t2[i][j] / t1[i][j] <= 1.05) {
					continue;
				}
				else {
					std::cout << t2[i][j] << " Ungleich " << t1[i][j] << " " << i << " " << j << std::endl;
					exit(-1);
				}

			}
		}
	}
	std::cout << "test ok" << std::endl;
}
/*
void matrix_multiplication_sse() {
#pragma omp parallel for
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			__m128 vAdd, vAdd1;
			vAdd = vAdd1 = _mm_setzero_ps();
			
			for (int k = 0; k < size; k+=4) {
				__m128 vA = _mm_load_ps(&aa[i][k]);
				__m128 vB = { bb[k][j], bb[k + 1][j], bb[k + 2][j], bb[k + 3][j] };
				//__m128 vB1 = { bb[k][j + 1], bb[k + 1][j + 1], bb[k + 2][j + 1], bb[k + 3][j + 1] };
				 vAdd = _mm_add_ps(_mm_mul_ps(vA, vB),vAdd);
				 //vAdd1 = _mm_add_ps(_mm_mul_ps(vA, vB1), vAdd1);																		 
			}
			__m128 vHdot = _mm_add_ps(vAdd, _mm_shuffle_ps(vAdd, vAdd, 01000100));
			__m128 dot = _mm_shuffle_ps(_mm_add_ps(_mm_shuffle_ps(vAdd, vHdot, 10111110), vHdot), vHdot, 11000011);
			__m128 vHdot1 = _mm_add_ps(vAdd1, _mm_shuffle_ps(vAdd1, vAdd1, 01000100));
			__m128 dot1 = _mm_shuffle_ps(_mm_add_ps(_mm_shuffle_ps(vAdd1, vHdot1, 10111110), vHdot1), vHdot1, 11000011);
			_mm_store_ss(&cc2[i][j], dot);
			_mm_store_ss(&cc2[i][j + 1], dot1);
			 
		}
	}

}
*/

void matrix_multiplication_2_sse() {
#pragma omp parallel for

	for (int j = 0; j < size; j += 8) {
		for (int i = 0; i < size; i += tz_128) {
			for (int k = 0; k < size; k += tz_128) {

				for (int ii = i; ii < i + tz_128; ii++) {
					__m128 vAdd, vAdd1, vAdd2, vAdd3;
					if (k == 0) {

						vAdd = vAdd1 = vAdd2 = vAdd3 = _mm_setzero_ps();
					}
					else {
						vAdd = _mm_load_ps(&cc6[ii][j]);
						vAdd1 = _mm_load_ps(&cc6[ii][j + 4]);
						//vAdd2 = _mm_load_ps(&cc[ii][])



					}
					for (int kk = k; kk < k + tz_128; kk++) {
						__m128 vA = _mm_set1_ps(aa[ii][kk]);
						__m128 vB = _mm_load_ps(&bb[kk][j]);
						__m128 vB1 = _mm_load_ps(&bb[kk][j + 4]);



						vAdd = _mm_fmadd_ps(vA, vB, vAdd);
						vAdd1 = _mm_fmadd_ps(vA, vB1, vAdd1);



					}
					_mm_store_ps(&cc6[ii][j], vAdd);
					_mm_store_ps(&cc6[ii][j + 4], vAdd1);
				}
			}
		}
	}
					
}






void matrix_multiplication_fast_avx_versuch2() {
	
#pragma omp parallel for
	
		for (int j = 0; j < size; j += 32) {
			for (int i = 0; i < size; i += tz_256) {
				for (int k = 0; k < size; k += tz_256) {

					for (int ii = i; ii < i + tz_256; ii++) {
						__m256 vAdd, vAdd1, vAdd2, vAdd3, vAdd4, vAdd5, vAdd6, vAdd7;
						if (k == 0) {

							vAdd = vAdd1 = vAdd2 = vAdd3 = vAdd4 = vAdd5 = vAdd6 = vAdd7 = _mm256_setzero_ps();
						}
						else {
							vAdd = _mm256_load_ps(&cc[ii][j]);
							vAdd1 = _mm256_load_ps(&cc[ii][j + 8]);
							vAdd2 = _mm256_load_ps(&cc[ii][j + 16]);
							vAdd3 = _mm256_load_ps(&cc[ii][j + 24]);
						

						}
						for (int kk = k; kk < k + tz_256; kk++) {
							__m256 vA = _mm256_set1_ps(aa[ii][kk]);
							__m256 vB = _mm256_load_ps(&bb[kk][j]);
							__m256 vB1 = _mm256_load_ps(&bb[kk][j + 8]);
							__m256 vB2 = _mm256_load_ps(&bb[kk][j + 16]);
							__m256 vB3 = _mm256_load_ps(&bb[kk][j + 24]);
							

							vAdd = _mm256_fmadd_ps(vA, vB, vAdd);
							vAdd1 = _mm256_fmadd_ps(vA, vB1, vAdd1);
							vAdd2 = _mm256_fmadd_ps(vA, vB2, vAdd2);
							vAdd3 = _mm256_fmadd_ps(vA, vB3, vAdd3);
						
						}
						_mm256_store_ps(&cc[ii][j], vAdd);
						_mm256_store_ps(&cc[ii][j + 8], vAdd1);
						_mm256_store_ps(&cc[ii][j + 16], vAdd2);
						_mm256_store_ps(&cc[ii][j + 24], vAdd3);
					

					}
				}
			}
		}
}

void matrix_multiplication_fast_avx512_versuch2() {

#pragma omp parallel for  

	for (int j = 0; j < size; j += 64) {
		for (int i = 0; i < size; i += tz_512) {
			for (int k = 0; k < size; k += tz_512) {

				for (int ii = i; ii < i + tz_512; ii+=4) {
					__m512 vAdd, vAdd1, vAdd2, vAdd3, vAdd4, vAdd5, vAdd6, vAdd7, vAdd8, vAdd9, vAdd10, vAdd11, vAdd12, vAdd13, vAdd14, vAdd15;
					if (k == 0) {

						vAdd = vAdd1 = vAdd2 = vAdd3 = vAdd4 = vAdd5 = vAdd6 = vAdd7 = vAdd8 = vAdd9 = vAdd10 = vAdd11 = vAdd12 = vAdd13 = vAdd14 = vAdd15 = _mm512_setzero_ps();
					}
					else {
						vAdd = _mm512_load_ps(&cc1[ii][j]);
						vAdd1 = _mm512_load_ps(&cc1[ii][j + 16]);
						vAdd2 = _mm512_load_ps(&cc1[ii][j + 32]);
						vAdd3 = _mm512_load_ps(&cc1[ii][j + 48]);

						vAdd4 = _mm512_load_ps(&cc1[ii+1][j]);
						vAdd5 = _mm512_load_ps(&cc1[ii+1][j + 16]);
						vAdd6 = _mm512_load_ps(&cc1[ii+1][j + 32]);
						vAdd7 = _mm512_load_ps(&cc1[ii+1][j + 48]);

						vAdd8 = _mm512_load_ps(&cc1[ii + 2][j]);
						vAdd9 = _mm512_load_ps(&cc1[ii + 2][j + 16]);
						vAdd10 = _mm512_load_ps(&cc1[ii + 2][j + 32]);
						vAdd11 = _mm512_load_ps(&cc1[ii + 2][j + 48]);

						vAdd12 = _mm512_load_ps(&cc1[ii + 3][j]);
						vAdd13 = _mm512_load_ps(&cc1[ii + 3][j + 16]);
						vAdd14 = _mm512_load_ps(&cc1[ii + 3][j + 32]);
						vAdd15 = _mm512_load_ps(&cc1[ii + 3][j + 48]);	   



						
						
					}
					
					for (int kk = k; kk < k + tz_512; kk++) {
						__m512 vA = _mm512_set1_ps(aa[ii][kk]);
						__m512 vA_1 = _mm512_set1_ps(aa[ii + 1][kk]);
						__m512 vA_2 = _mm512_set1_ps(aa[ii + 2][kk]);
						__m512 vA_3 = _mm512_set1_ps(aa[ii + 3][kk]);
						__m512 vB = _mm512_load_ps(&bb[kk][j]);
						__m512 vB1 = _mm512_load_ps(&bb[kk][j + 16]);
						__m512 vB2 = _mm512_load_ps(&bb[kk][j + 32]);
						__m512 vB3 = _mm512_load_ps(&bb[kk][j + 48]);
					

					
						
						


						vAdd = _mm512_fmadd_ps(vA, vB, vAdd);
						vAdd1 = _mm512_fmadd_ps(vA, vB1, vAdd1);
						vAdd2 = _mm512_fmadd_ps(vA, vB2, vAdd2);
						vAdd3 = _mm512_fmadd_ps(vA, vB3, vAdd3);

						vAdd4 = _mm512_fmadd_ps(vA_1, vB, vAdd4);
						vAdd5 = _mm512_fmadd_ps(vA_1, vB1, vAdd5);
						vAdd6 = _mm512_fmadd_ps(vA_1, vB2, vAdd6);
						vAdd7 = _mm512_fmadd_ps(vA_1, vB3, vAdd7);

						vAdd8 = _mm512_fmadd_ps(vA_2, vB, vAdd8);
						vAdd9 = _mm512_fmadd_ps(vA_2, vB1, vAdd9);
						vAdd10 = _mm512_fmadd_ps(vA_2, vB2, vAdd10);
						vAdd11 = _mm512_fmadd_ps(vA_2, vB3, vAdd11);

						vAdd12 = _mm512_fmadd_ps(vA_3, vB, vAdd12);
						vAdd13 = _mm512_fmadd_ps(vA_3, vB1, vAdd13);
						vAdd14 = _mm512_fmadd_ps(vA_3, vB2, vAdd14);
						vAdd15 = _mm512_fmadd_ps(vA_3, vB3, vAdd15);	

						

					}
					_mm512_store_ps(&cc1[ii][j], vAdd);
					_mm512_store_ps(&cc1[ii][j + 16], vAdd1);
					_mm512_store_ps(&cc1[ii][j + 32], vAdd2);
					_mm512_store_ps(&cc1[ii][j + 48], vAdd3);

					_mm512_store_ps(&cc1[ii + 1][j], vAdd4);
					_mm512_store_ps(&cc1[ii + 1][j + 16], vAdd5);
					_mm512_store_ps(&cc1[ii + 1][j + 32], vAdd6);
					_mm512_store_ps(&cc1[ii + 1][j + 48], vAdd7);

					_mm512_store_ps(&cc1[ii + 2][j], vAdd8);
					_mm512_store_ps(&cc1[ii + 2][j + 16], vAdd9);
					_mm512_store_ps(&cc1[ii + 2][j + 32], vAdd10);
					_mm512_store_ps(&cc1[ii + 2][j + 48], vAdd11);

					_mm512_store_ps(&cc1[ii + 3][j], vAdd12);
					_mm512_store_ps(&cc1[ii + 3][j + 16], vAdd13);
					_mm512_store_ps(&cc1[ii + 3][j + 32], vAdd14);
					_mm512_store_ps(&cc1[ii + 3][j + 48], vAdd15);		  

				
					
				


				}
			}
		}
	}
}	



		






int main()
{
	
	create_random_matrix();
	
	
	
	double time_spent = 0;
	double time_spent1 = 0;
	double time_spent3 = 0;
	double time_spent4 = 0;
	double time_spent5 = 0;

	 
	for (int i = 0; i < 1; i++) {
		clock_t begin = clock();
		matrix_multiplication_fast_avx_versuch2();
		clock_t end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		
		
	}
	std::cout << "AVX-256: " << time_spent/1 << std::endl;
	
		  
	for (int i = 0; i < 1; i++) {
		clock_t begin1 = clock();
		matrix_multiplication_fast_avx512_versuch2();
		clock_t end1 = clock();
		time_spent1 += (double)(end1 - begin1) / CLOCKS_PER_SEC;
		
	}
	std::cout << "AVX-512: " << time_spent1/1 << std::endl;
	  
	for (int i = 0; i < 1; i++) {
		clock_t begin3 = clock();
		matrix_multiplication_2_sse();
		clock_t end3 = clock();
		time_spent3 += (double)(end3 - begin3) / CLOCKS_PER_SEC;
		
	}
	std::cout << "2 SSE: " << time_spent3/1 << std::endl;


	 /*
	for (int i = 0; i < 1; i++) {
		clock_t begin4 = clock();
		matrix_multiplication_reihenweise();
		clock_t end4 = clock();
		time_spent4 += (double)(end4 - begin4) / CLOCKS_PER_SEC;
	}
	std::cout << "Row-wise: " << time_spent4/1 << std::endl;

	for (int i = 0; i < 1; i++) {
		clock_t begin5 = clock();
		matrix_multiplication();
		clock_t end5 = clock();
		time_spent5 += (double)(end5 - begin5) / CLOCKS_PER_SEC;
	}
	std::cout << "Naive: " << time_spent5/1 << std::endl;


	//test(cc, cc6);	*/ 
	



	 
	
	 				 
		

}