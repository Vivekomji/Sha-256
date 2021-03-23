#include <bits/stdc++.h>
#define ll long long
using namespace std;
const ll MOD = 1LL<<32; //defining a variable MOD with value 2^32

class SHA_256
{
	private :
		
		vector<int> prime;             // container for first 64 prime numbers
		vector<ll> H = vector<ll>(8);  // to store initial Hash Values
		vector<ll> K = vector<ll>(64); // to store initial Round Constants
		vector<ll> W = vector<ll>(64); // to store words of the padded message block
		string paddedMessage;          // stores the message after padding
		string res;                    // to store the final hash values
		
		ll binary_to_decimal(string S) // Function to convert binary numbers into decimal
		{
			ll res = 0;                // initialization of result of the function

			reverse(S.begin(),S.end());

			for(int i=0; i<(int)S.length(); i++)
				res += ((S[i] - '0') * (1LL<<i));

			return res;
		}
		
		string decimal_to_hexadecimal(ll x) // Function to convert decimal number into hexa-decimal
		{
			string res(8,'0');
			for(int i=7; i>=0; i--)
			{
				if(x%16 < 10)
					res[i] = char('0' + x%16);
				else
					res[i] = char('a' + x%16 - 10);
				x /= 16;
			}
			return res;
		}
		
	// helper functions to be used in the implementation of algorithm
		ll RR(unsigned int x, unsigned int y) // to Right-Rotate x by y bits
		{
			return (x >> y)|(x << (32-y));
		}
		
		ll RS(unsigned int x, unsigned int y) // to Right Shift x by y bits
		{
			return (x >> y);
		}
		
		ll Ch(ll x, ll y, ll z) // Choice function
		{
			return ((x & y) ^ ((MOD-1-x) & z));
		}
		
		ll Maj(ll x, ll y, ll z) // Majority function
		{
			return ((x & y) ^ (x & z) ^ (y & z));
		}
		
		ll Summ0(ll x) // Summation0 function
		{
			return (RR(x,2) ^ RR(x,13) ^ RR(x,22));
		}
		
		ll Summ1(ll x) // Summation1 function
		{
			return (RR(x,6) ^ RR(x,11) ^ RR(x,25));
		}
		
		ll Sigma0(ll x) // Sigma0 function
		{
			return (RR(x,7) ^ RR(x,18) ^ RS(x,3));
		}
		
		ll Sigma1(ll x) // Sigma1 function
		{
			return (RR(x,17) ^ RR(x,19) ^ RS(x,10));
		}
		
	// function to be used in constructor
		void get_First_64_Primes() // to generate first 64 prime numbers
		{
			function<bool (int)> isPrime = [&](int x)
			{
				// if even it cannot be prime
				if(x < 2)
					return false;
				
				for(int j=2; j<=sqrt(x); j++)
					if(x%j == 0)
						return false;
		 
				return true;
			};
			for(int i=2; (int)prime.size()<64; i++)
				if(isPrime(i))
					prime.push_back(i);
		}
		
		// The initial hash values are the fractional part of the square root of first 8 primes
		void initialise_Hash_Values() // calculates the initial Hash Values
		{
			for(int i=0; i<8; i++)
			{
				double x = (sqrt(prime[i]) - floor(sqrt(prime[i]))) * MOD;
				H[i] = (ll)x;
			}
		}
		
		// The initial round constants are the fractional part of the cube root of first 64 primes
		void initialise_Round_Constants() // calculates the initial Round Constants
		{
			for(int i=0; i<64; i++)
			{
				double x = (cbrt(prime[i]) - floor(cbrt(prime[i]))) * MOD;
				K[i] = (ll)x;
			}
		}

	public :
	
		SHA_256() // constructor
		{
			get_First_64_Primes();
			initialise_Hash_Values();
			initialise_Round_Constants();
		}
		
		void padding_The_Message(string message) // step-1 : padding
		{
			paddedMessage = "";
			
			// converting each character of the message into binary using inbuilt 'bitset'
			for(auto c : message)
				paddedMessage += bitset<8> (int(c)).to_string();
		 
			// appending one
			paddedMessage += "1";
		 
			// appending zeros
			paddedMessage += string((512 - 8*(int)message.length() - 64 - 1), '0');
		 
			// appending the last 64 bits which are the binary of 8*(int)message.length()
			paddedMessage += bitset<64> (8*(int)message.length()).to_string();
		}
		
		void parsing_The_Padded_Message() // step-2 : parsing
		{
			vector<ll> M(16);
			for(int i=0; i<(int)paddedMessage.length(); i+=32)
			{
				string T = paddedMessage.substr(i,32);
				M[i/32] = binary_to_decimal(T);
			}
		 
			for(int t=0; t<64; t++)
			{
				if(t <= 15)
					W[t] = M[t];
				else
					W[t] = (Sigma1(W[t-2]) + W[t-7] + Sigma0(W[t-15]) + W[t-16]) % MOD;
			}
		}
		
		void final_Hash() // step-3 : calculate hash
		{
			ll a, b, c, d, e, f, g, h, t1, t2;
		 
			a = H[0];
			b = H[1];
			c = H[2];
			d = H[3];
			e = H[4];
			f = H[5];
			g = H[6];
			h = H[7];
		 
			for(int t=0; t<64; t++)
			{
				t1 = (h + Summ1(e) + Ch(e,f,g) + K[t] + W[t]) % MOD;
				t2 = (Summ0(a) + Maj(a,b,c)) % MOD;
				h = g;
				g = f;
				f = e;
				e = (d + t1) % MOD;
				d = c;
				c = b;
				b = a;
				a = (t1 + t2) % MOD;
			}
		 
			res += decimal_to_hexadecimal((a+H[0])%MOD);
			res += decimal_to_hexadecimal((b+H[1])%MOD);
			res += decimal_to_hexadecimal((c+H[2])%MOD);
			res += decimal_to_hexadecimal((d+H[3])%MOD);
			res += decimal_to_hexadecimal((e+H[4])%MOD);
			res += decimal_to_hexadecimal((f+H[5])%MOD);
			res += decimal_to_hexadecimal((g+H[6])%MOD);
			res += decimal_to_hexadecimal((h+H[7])%MOD);
		}
		
		string Hash_Value(string& message)
		{
			res = "";
			
			padding_The_Message(message);
			parsing_The_Padded_Message();
			final_Hash();
			
			return res;
		}
};
 
int main() 
{
	SHA_256 obj;
	
	string message;
	cout<<"Enter data to be hashed: "<<endl;
	getline(cin,message);
	cout << message <<  " : " << obj.Hash_Value(message) << endl;
 
	return 0;
}
