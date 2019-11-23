#include <iostream>
#include <json-c/json.h>
#include <vector>

using namespace std;

int main(int argc, char **argv)

{
	FILE *fp;
	char buffer[4000];
	struct json_object *parsed_json;
	struct json_object *points;
	struct json_object *point;
	struct json_object *x;
	struct json_object *y;

	size_t n_points;
	size_t n_point;

	struct A_point {
		int x;
		int y;
	};

	size_t i;	

	std::vector<A_point> points_vector;

	fp = fopen("test1.json","r");
	fread(buffer, 4000, 1, fp);
	fclose(fp);

	parsed_json = json_tokener_parse(buffer);

	json_object_object_get_ex(parsed_json, "points", &points);

	// printf("Name: %s\n", json_object_get_string(name));
	// printf("Age: %d\n", json_object_get_int(age));

	n_points = json_object_array_length(points);
	printf("Found %lu points\n",n_points);

	for(i=0;i<n_points;i++) {
		point = json_object_array_get_idx(points, i);
		json_object_object_get_ex(point, "x", &x);
		json_object_object_get_ex(point, "y", &y);
		struct A_point temp_point = A_point({json_object_get_int(x), json_object_get_int(y)});
		points_vector.push_back(temp_point);
	}	
	cout << points_vector.size();

	for(i=0; i<points_vector.size(); i++){
		cout << points_vector[i].x << "\n";
	}

}
