#include <iostream>
#include <intrin.h>
#include <stdio.h>



int const size = 512;
float A[size][size];
float B[size][size];
float C[size][size];
float D[size][size];


float randomFloat()
{
	return (float)(rand()) / (float)(rand());
}
void create_random_matrix() 
{
	//float** A = (float**)malloc(500 * 500 * sizeof(float*));
	//A[0][0] = 3.0;
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			A[i][j] = randomFloat();
		}
	}
	
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			B[i][j] = randomFloat();
		}
	}
	std::cout << size << std::endl;
	
	//std::cout << A[0][0] << std::endl;

}

void matrix_multiplication()
{
	for (int k = 0; k < size; k++) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				C[k][i] += A[k][j] * B[j][i];
				
			}
			
		}
	}
}
void matrix_multiplication_reihenweise()
{
	for (int k = 0; k < size; k++) {
		for (int j = 0; j < size; j++) {
			for (int i = 0; i < size; i++) {
				C[k][i] += A[k][j] * B[j][i];
				

			}

		}
	}
}
void matrix_multiplication_xmm() {
	


	for (int k = 0; k < size; k++) {
		for (int j = 0; j < size; j += 4) {
			for (int i = 0; i < size; i++) {
				__m128 v0 = { A[k][j], A[k][j + 1], A[k][j + 2], A[k][j + 3] };
				__m128 v1 = { B[j][i], B[j + 1][i], B[j + 2][i], B[j + 3][i] };
				//__m128 v7 = { B[j][i + 1], B[j + 1][i + 1], B[j + 2][i + 1], B[j + 3][i + 1] };
				__m128 v2 = _mm_mul_ps(v0, v1);
				v1 = _mm_shuffle_ps(v2, v2, 01000100);
				v1 = _mm_add_ps(v1, v2);
				v2 = _mm_shuffle_ps(v1, v1, 10100100);
				v2 = _mm_add_ps(v1, v2);
				//v2 = _mm_shuffle_ps(v2, v2, 11100111);
				float d[4];

				_mm_storeu_ps(d, v2);
				C[k][i] += d[3];
				//std::cout << C[k][i] << std::endl;
				

				//std::cout << d[0] << std::endl;
				//std::cout << d[1] << std::endl;
				//std::cout << d[2] << std::endl;
				//std::cout << d[3] << std::endl;
				//std::cout << "here" << std::endl;
				
				//break;
			}
			
			//break;

		}
		//break;
	}

}
void matrix_multiplication_ymm(){	   
	
	


	for (int k = 0; k < size; k++) {
		
		for (int m = 0; m < size; m += 3) {
			__m256 c_dot = { 0,0,0,0,0,0,0,0 };
			__m256 c1_dot = { 0,0,0,0,0,0,0,0 };
			__m256 c2_dot = { 0,0,0,0,0,0,0,0 };
			for (int j = 0; j < size; j += 16) {

				for (int i = m; i < m + 3; i++) {
					__m256 a = { A[k][j], A[k][j + 1], A[k][j + 2], A[k][j + 3], A[k][j + 4], A[k][j + 5], A[k][j + 6], A[k][j + 7] };
					__m256 b = { B[j][i],B[j + 1][i],B[j + 2][i],B[j + 3][i],B[j + 4][i],B[j + 5][i],B[j + 6][i],B[j + 7][i] };
					__m256 c = { A[k][j + 8],A[k][j + 9],A[k][j + 10],A[k][j + 11],A[k][j + 12],A[k][j + 13],A[k][j + 14],A[k][j + 15] };
					__m256 d = { B[j + 8][i],B[j + 9][i],B[j + 10][i],B[j + 11][i],B[j + 12][i],B[j + 13][i],B[j + 14][i],B[j + 15][i] };
					//__m256 a = { 13,23,33,43,53,63,73,83 };
					//__m256 b = { 14,24,34,44,54,64,74,84 };
					//__m256 c = { 15,25,35,45,55,65,75,85 };
					//__m256 d = { 16,26,36,46,56,66,76,86 };
					__m256 a_b = _mm256_mul_ps(a, b);
					__m256 c_d = _mm256_mul_ps(c, d);
					
					//__m256 xy03 = _mm256_mul_ps(e, f);
					//__m256 xy04 = _mm256_mul_ps(g, h);

		//__m512i v13 = _mm512_dpwssds_epi32(v10, v11, v12);


		// low to high: xy00+xy01 xy10+xy11 xy02+xy03 xy12+xy13
					__m256 a_c_b_d = _mm256_hadd_ps(a_b, c_d);

					// low to high: xy20+xy21 xy30+xy31 xy22+xy23 xy32+xy33
				   // __m256 temp23 = _mm256_hadd_ps(xy03, xy04);

					//__m256 temp24 = _mm256_hadd_ps(temp22, xy03);

					__m256 temp = _mm256_hadd_ps(a_c_b_d, a_c_b_d);

					// low to high: xy02+xy03 xy12+xy13 xy20+xy21 xy30+xy31
					__m256 swapped = _mm256_permute2f128_ps(temp, temp, 0x21);

					// low to high: xy00+xy01 xy10+xy11 xy22+xy23 xy32+xy33


					__m256 dotproduct = _mm256_add_ps(swapped, temp);
					__m256 dotproducts = _mm256_hadd_ps(dotproduct, dotproduct);
					if (i == m) {
						c_dot = _mm256_add_ps(dotproducts, c_dot);

					}
					else if (i == m + 1) {
						c1_dot = _mm256_add_ps(dotproducts, c1_dot);

					}
					else if (i == m + 2) {
						c2_dot = _mm256_add_ps(dotproducts, c2_dot);

					}
				


				}
			}
			float t[8];
			_mm256_store_ps(t, c_dot);
			C[k][m] = t[0];
			_mm256_store_ps(t, c1_dot);
			C[k][m + 1] = t[0];
			_mm256_store_ps(t, c2_dot);
			C[k][m + 2] = t[0];
		}
	}


	//float m[8];
	//_mm256_store_ps(m, dotproducts);

	// std::cout << m[0] << std::endl;
	 //std::cout << m[1] << std::endl;
	 //std::cout << m[2] << std::endl;
	 //std::cout << m[3] << std::endl;
	 //std::cout << m[4] << std::endl;
	 //std::cout << m[5] << std::endl;
	 //std::cout << m[6] << std::endl;
	 //std::cout << m[7] << std::endl;

}

void clear_matrix() {
	for (int i = 0; i < size; i++) {
		std::fill_n(C[i], size, 0);
	}

}

void matrix_multiplication_fast_avx() {
	int T = 6;
	int begin[6];
	int end[6];
	for (int i = 0; i < T; i++) {
		begin[i] = (size / T * i) + 1;
		end[i] = size / T * (i + 1);
		std::cout << begin[i] << std::endl;
		std::cout << end[i] << std::endl;
	}
	for (int k = 0; k < size; k++) {
		for (int i = 0; i < size; i+= 8) {
			for (int j = 0; j < size; j+=6) {
				__m256 v0 = { A[k][i],A[k][i+1], A[k][i+2], A[k][i+3], A[k][i+4], A[k][i+5], A[k][i+6], A[k][i+7] };
				__m256 v1 = { B[i][j],B[i+1][j],B[i+2][j],B[i+3][j],B[i+4][j],B[i+5][j],B[i+6][j],B[i+7][j] };
				__m256 v2 = { B[i][j+1],B[i + 1][j+1],B[i + 2][j+1],B[i + 3][j+1],B[i + 4][j+1],B[i + 5][j+1],B[i + 6][j+1],B[i + 7][j+1] };
				__m256 v3 = { B[i][j + 2],B[i + 1][j + 2],B[i + 2][j + 2],B[i + 3][j + 2],B[i + 4][j + 2],B[i + 5][j + 2],B[i + 6][j + 2],B[i + 7][j + 2] };
				__m256 v4 = { B[i][j + 3],B[i + 1][j + 3],B[i + 2][j + 3],B[i + 3][j + 3],B[i + 4][j + 3],B[i + 5][j + 3],B[i + 6][j + 3],B[i + 7][j + 3] };
				__m256 v5 = { B[i][j + 4],B[i + 1][j + 4],B[i + 2][j + 4],B[i + 3][j + 4],B[i + 4][j + 4],B[i + 5][j + 4],B[i + 6][j + 4],B[i + 7][j + 4] };
				//__m256 v6 = { B[i][j + 5],B[i + 1][j + 5],B[i + 2][j + 5],B[i + 3][j + 5],B[i + 4][j + 5],B[i + 5][j + 5],B[i + 6][j + 5],B[i + 7][j + 5] };
				//__m256 v7 = { B[i][j + 6],B[i + 1][j + 6],B[i + 2][j + 6],B[i + 3][j + 6],B[i + 4][j + 6],B[i + 5][j + 6],B[i + 6][j + 6],B[i + 7][j + 6] };
				int aa = 0;
				__m256 v_dp_1 = _mm256_dp_ps(v0, v1, 0xff);
				__m256 v_permute_1 = _mm256_permute2f128_ps(v_dp_1, v_dp_1, 0x21);
				__m256 v_result_1 = _mm256_add_ps(v_dp_1, v_permute_1);
				__m256 v_dp_2 = _mm256_dp_ps(v0, v2, 255);
				__m256 v_permute_2 = _mm256_permute2f128_ps(v_dp_2, v_dp_2, 0x21);
				__m256 v_result_2 = _mm256_add_ps(v_dp_2, v_permute_2);
				__m256 v_dp_3 = _mm256_dp_ps(v0, v3, 255);
				__m256 v_permute_3 = _mm256_permute2f128_ps(v_dp_3, v_dp_3, 0x21);
				__m256 v_result_3 = _mm256_add_ps(v_dp_3, v_permute_3);
				__m256 v_dp_4 = _mm256_dp_ps(v0, v4, 255);
				__m256 v_permute_4 = _mm256_permute2f128_ps(v_dp_4, v_dp_4, 0x21);
				__m256 v_result_4 = _mm256_add_ps(v_dp_4, v_permute_4);
				__m256 v_dp_5 = _mm256_dp_ps(v0, v5, 255);
				__m256 v_permute_5 = _mm256_permute2f128_ps(v_dp_5, v_dp_5, 0x21);
				__m256 v_result_5 = _mm256_add_ps(v_dp_5, v_permute_5);

				
				//__m256 v_dp_6 = _mm256_dp_ps(v0, v6, 255);
				//__m256 v_permute_6 = _mm256_permute2f128_ps(v_dp_6, v_dp_6, 0x21);
				//__m256 v_result_6 = _mm256_add_ps(v_dp_6, v_permute_6);
				//__m256 v_dp_7 = _mm256_dp_ps(v0, v7, 255);
				//__m256 v_permute_7 = _mm256_permute2f128_ps(v_dp_7, v_dp_7, 0x21);
				//__m256 v_result_7 = _mm256_add_ps(v_dp_7, v_permute_7);
				float t[8];
				float u[8];
				float m[8];
				float n[8];
				float o[8];
				//float p[8];
				//float q[8];
				_mm256_store_ps(t, v_result_1);
				_mm256_store_ps(u, v_result_2);
				_mm256_store_ps(m, v_result_3);
				_mm256_store_ps(n, v_result_4);
				_mm256_store_ps(o, v_result_5);
				//_mm256_store_ps(p, v_result_6);
				//_mm256_store_ps(q, v_result_7);
				
				
				C[k][j] += t[0];
				C[k][j + 1] += u[0];
				C[k][j + 2] += m[0];
				C[k][j + 3] += n[0];
				C[k][j + 4] += o[0];
				//C[k][j + 5] += p[0];
				//C[k][j + 6] += q[0];

			}
			

		}
		
	}
	


}


int main()
{
	// seeds the generator
	
	 create_random_matrix();
	// matrix_multiplication();
	// std::cout << C[0][400] << std::endl;
	 //clear_matrix();
	 matrix_multiplication_reihenweise();
	 std::cout << C[0][355] << std::endl;
	 //clear_matrix();
	 //matrix_multiplication_xmm();
	 //std::cout << C[0][395] << std::endl;
	 //clear_matrix();
	 //matrix_multiplication_ymm();
	 //std::cout << C[0][2] << std::endl;

	 clear_matrix();
	 matrix_multiplication_fast_avx();
	 std::cout << C[0][355] << std::endl;
	 
	 
	 
	 
	 

	// __m256 v0 = { A[0][0],A[0][1],A[0][2],A[0][3],A[0][4],A[0][5],A[0][6],A[0][7] };
	// __m256 v1 = { B[0][0],B[1][0],B[2][0],B[3][0],B[4][0],B[5][0],B[6][0],B[7][0] };
	 
	 
	
}