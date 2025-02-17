#include <vtkAutoInit.h> 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCubeSource.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOctreePointLocator.h>
#include <vtkOutlineFilter.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include<vtkPolyLine.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOctreePointLocator.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkSphereSource.h>
#include <vtkSTLReader.h>
#include <vtkTextProperty.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetEventTranslator.h>
#include<vtkPointSource.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vtkTriangleFilter.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkOctreePointLocator.h>
#include <vtkPolyData.h>
#include <vtkIdList.h>
#include <iostream>
#include <set>
#include <vtkCursor3D.h>
#include <vtkMath.h>
#include <vtkMath.h>
#include<vtkSelectEnclosedPoints.h>
#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkExtractVOI.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <new.h>
#include<stack>
#include <chrono>
#include <unordered_set>


using namespace std;

//GLOBAL VARIABLES

long long int maxoct;
//array that stores all bottom octants voxel id..
std::vector<int> bottom;
//map that map voxelid to voxeltype and its center cordinate..
std::map<int, std::pair<char, std::vector<double>>> bmap;
double minbox[3], maxbox[3];



int totalchlild = 0;





void GetOctantVertices(double minBounds[3], double maxBounds[3], double vertices[8][3])
{
	// Extract the minimum and maximum coordinates of the bounding box
	double xmin = minBounds[0];
	double ymin = minBounds[1];
	double zmin = minBounds[2];
	double xmax = maxBounds[0];
	double ymax = maxBounds[1];
	double zmax = maxBounds[2];

	// Calculate the eight vertices of the octant
	vertices[0][0] = xmin;
	vertices[0][1] = ymin;
	vertices[0][2] = zmin;

	vertices[1][0] = xmax;
	vertices[1][1] = ymin;
	vertices[1][2] = zmin;

	vertices[2][0] = xmax;
	vertices[2][1] = ymax;
	vertices[2][2] = zmin;

	vertices[3][0] = xmin;
	vertices[3][1] = ymax;
	vertices[3][2] = zmin;

	vertices[4][0] = xmin;
	vertices[4][1] = ymin;
	vertices[4][2] = zmax;

	vertices[5][0] = xmax;
	vertices[5][1] = ymin;
	vertices[5][2] = zmax;

	vertices[6][0] = xmax;
	vertices[6][1] = ymax;
	vertices[6][2] = zmax;

	vertices[7][0] = xmin;
	vertices[7][1] = ymax;
	vertices[7][2] = zmax;
}

void filltest(double x, double y, double z, double test[3]) {
	test[0] = x;
	test[1] = y;
	test[2] = z;
}

//left right front back  down
//current voxel id and bmap

//created for left voxel return type
struct VoxelData {
	int voxelId;
	double centerX;
	double centerY;
	double centerZ;
};


VoxelData leftvoxel(int voxelid, double center[3], int voxelsize) {
	VoxelData ans;
	ans.voxelId = voxelid - 1;
	ans.centerX = center[0] - voxelsize;
	ans.centerY = center[1];
	ans.centerZ = center[2];

	//if left voxel does not exist
	if (ans.centerX < minbox[0]) {
		ans.voxelId = -100;
	}

	return ans;

}


//right
//right
VoxelData rightvoxel(int voxelid, double center[3], int voxelsize) {
	VoxelData ans;
	ans.voxelId = voxelid + 1;
	ans.centerX = center[0] + voxelsize;
	ans.centerY = center[1];
	ans.centerZ = center[2];

	//if right voxel does not exist
	if (ans.centerX > maxbox[0]) {
		ans.voxelId = -100;
	}

	return ans;

}

//bottom

VoxelData bottomvoxel(int voxelid, double center[3], int voxelsize) {
	VoxelData ans;
	int minus = std::cbrt(maxoct);
	ans.voxelId = voxelid - minus;
	ans.centerX = center[0];
	ans.centerY = center[1] - voxelsize;
	ans.centerZ = center[2];

	//if bottom voxel does not exist
	if (ans.centerY < minbox[1]) {
		ans.voxelId = -100;
	}

	return ans;
}

//top

VoxelData topvoxel(int voxelid, double center[3], int voxelsize) {
	VoxelData ans;
	int plus = std::cbrt(maxoct);
	ans.voxelId = voxelid + plus;
	ans.centerX = center[0];
	ans.centerY = center[1] + voxelsize;
	ans.centerZ = center[2];

	//if bottom voxel does not exist
	if (ans.centerY > maxbox[1]) {
		ans.voxelId = -100;
	}

	return ans;
}



//front

VoxelData frontvoxel(int voxelid, double center[3], int voxelsize) {
	VoxelData ans;
	int minus = std::cbrt(maxoct);
	minus = minus * minus;
	ans.voxelId = voxelid - minus;
	ans.centerX = center[0];
	ans.centerY = center[1];
	ans.centerZ = center[2] - voxelsize;

	//if bottom voxel does not exist
	if (ans.centerZ < minbox[2]) {
		ans.voxelId = -100;
	}

	return ans;
}

//back
VoxelData backvoxel(int voxelid, double center[3], int voxelsize) {
	VoxelData ans;
	int plus = std::cbrt(maxoct);
	plus = plus * plus;
	ans.voxelId = voxelid + plus;
	ans.centerX = center[0];
	ans.centerY = center[1];
	ans.centerZ = center[2] + voxelsize;

	//if bottom voxel does not exist
	if (ans.centerZ > maxbox[2]) {
		ans.voxelId = -100;
	}

	return ans;
}


//tells the position of voxel inside outside or partial
//voxelid = node
int position(double cord[3], int voxelid, int voxelsize, vtkPolyData* shape) {
	//ans =1  inside
	//ans=2  partial
	//ans=3   outside

	int ans = 0;
	double center[3];
	filltest(cord[0], cord[1], cord[2], center);
	//drawing for 60th voxel
	//insdie,outside,partial
	double minb[3], maxb[3];
	minb[0] = cord[0] - ((0.5) * voxelsize);
	minb[1] = cord[1] - ((0.5) * voxelsize);
	minb[2] = cord[2] - ((0.5) * voxelsize);
	//cout << "minbbbb " << minb[0] << " " << minb[1] << " " << minb[2] << endl;
	maxb[0] = cord[0] + ((0.5) * voxelsize);
	maxb[1] = cord[1] + ((0.5) * voxelsize);
	maxb[2] = cord[2] + ((0.5) * voxelsize);

	double vertices[8][3];
	GetOctantVertices(minb, maxb, vertices);

	//inside and outside vertex count....
	int inside = 0;
	int outside = 0;


	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	double test0[3];
	double test1[3];
	double test2[3];
	double test3[3];
	double test4[3];
	double test5[3];
	double test6[3];
	double test7[3];


	filltest(vertices[0][0], vertices[0][1], vertices[0][2], test0);
	points->InsertNextPoint(test0);
	filltest(vertices[1][0], vertices[1][1], vertices[1][2], test1);
	points->InsertNextPoint(test1);
	filltest(vertices[2][0], vertices[2][1], vertices[2][2], test2);
	points->InsertNextPoint(test2);
	filltest(vertices[3][0], vertices[3][1], vertices[3][2], test3);
	points->InsertNextPoint(test3);
	filltest(vertices[4][0], vertices[4][1], vertices[4][2], test4);
	points->InsertNextPoint(test4);
	filltest(vertices[5][0], vertices[5][1], vertices[5][2], test5);
	points->InsertNextPoint(test5);
	filltest(vertices[6][0], vertices[6][1], vertices[6][2], test6);
	points->InsertNextPoint(test6);
	filltest(vertices[7][0], vertices[7][1], vertices[7][2], test7);
	points->InsertNextPoint(test7);

	//cout << "test0" << test0[0] << " " << test0[1] << " " << test0[2] << endl;


	/*vtkSmartPointer<vtkPoints> points1 = vtkSmartPointer<vtkPoints>::New();
	double test[3] = {2.5,5.5,5.5};
	points1->InsertNextPoint(test);
	vtkSmartPointer<vtkPolyData> pointsPolydata1 = vtkSmartPointer<vtkPolyData>::New();;
	pointsPolydata1->SetPoints(points1);
	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints1 = vtkSmartPointer<vtkSelectEnclosedPoints>::New();;
	selectEnclosedPoints1->SetInputData(pointsPolydata1);
	selectEnclosedPoints1->SetSurfaceData(shape);
	selectEnclosedPoints1->Update();
	if (selectEnclosedPoints1->IsInside(0) == 1) cout << "iiiiiiinside";
	else cout << "outsideeeee";*/

	vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
	pointsPolydata->SetPoints(points);
	vtkSmartPointer<vtkSelectEnclosedPoints> selectEnclosedPoints = vtkSmartPointer<vtkSelectEnclosedPoints>::New();;
	selectEnclosedPoints->SetInputData(pointsPolydata);
	selectEnclosedPoints->SetSurfaceData(shape);
	selectEnclosedPoints->Update();

	for (unsigned int xx = 0; xx < 8; xx++) {

		if (selectEnclosedPoints->IsInside(xx) == 1)
		{
			inside++;
		}
		else
		{
			outside++;
		}
	}

	std::vector<double> c{ cord[0],cord[1],cord[2] };


	if (inside == 8) {
		ans = 1;
		//cout << "insideoctant" <<voxelid << endl;
		bmap[voxelid] = std::make_pair('I', c);
		//cout << "did this one inside voxelid===== " << voxelid << endl;
	}
	else {
		if (outside == 8) {
			ans = 3;
			//cout << "outsideocatnat"<<voxelid << endl;
			bmap[voxelid] = std::make_pair('O', c);
			//cout << "did this one outside voxelid===== " << voxelid << endl;
		}
		else {
			ans = 2;
			//cout << "partialoctant                        "<<voxelid<< endl;
			bmap[voxelid] = std::make_pair('P', c);
			//cout << "did this one partial voxelid===== " << voxelid << endl;

			// 1 posistion call of bottomvoxel extra here so commenting it..
			/*int xxx = bottomvoxel(voxelid, voxelsize, bmap, maxoct);
			if (xxx != -1) {
				char c = bmap[xxx].first;
				if (c == 'O') bottom.push_back(voxelid);
			}*/
		}
	}

	return ans;
}

vector<VoxelData> adjacent(int node, double center[3], int voxelsize) {
	vector<VoxelData> adj;

	VoxelData left = leftvoxel(node, center, voxelsize);
	if (left.voxelId != -100) {
		adj.push_back(left);
	}

	//right
	VoxelData right = rightvoxel(node, center, voxelsize);
	if (right.voxelId != -100) {
		adj.push_back(right);
	}

	VoxelData top = topvoxel(node, center, voxelsize);
	if (top.voxelId != -100) {
		adj.push_back(top);
	}

	VoxelData bottom = bottomvoxel(node, center, voxelsize);
	if (bottom.voxelId != -100) {
		adj.push_back(bottom);
	}

	VoxelData front = frontvoxel(node, center, voxelsize);
	if (front.voxelId != -100) {
		adj.push_back(front);
	}

	VoxelData back = backvoxel(node, center, voxelsize);
	if (back.voxelId != -100) {
		adj.push_back(back);
	}

	return adj;
}




//dfs(start, adj, vis, insidels, partialls);
//dfs function returns list of nodes that are internal or partial... 
void dfs(int node, vector<int>& vis, vector<int>& insidels, vector<int>& partialls, int voxelsize, vtkPolyData* shape) {

	stack<int> s;
	s.push(node);

	while (!s.empty()) {
		int var = s.top();
		s.pop();

		if (!vis[var])
		{
			vis[var] = 1;

			std::vector<double> center = bmap[var].second;
			double cen[3] = { center[0],center[1],center[2] };

			int ans = position(cen, var, voxelsize, shape);
			//	cout << "posistion is =======================================" << ans << endl;
			if (ans == 1) insidels.push_back(var);
			if (ans == 2) partialls.push_back(var);
			if (ans == 3) continue;
			else {
				vector<VoxelData> adj = adjacent(var, cen, voxelsize);

				for (const auto& it : adj) {
					if (!vis[it.voxelId]) {
						s.push(it.voxelId);
					}
				}
			}
		}
	}
}



//function to get all the bottom partial voxels from partial voxels
vector<int> getbottompartial(vector<int> partialls) {
	vector<int> bottom;
	int minus = std::cbrt(maxoct);

	for (auto it : partialls) {
		//no need to check for existence as bottom voxel of every partial voxel is added into adjacent list so with posistion function its bmap is added if it exist and existence of bottom voxel check in adjacent list
		if (bmap[it - minus].first == 'O') bottom.push_back(it);
	}

	return bottom;

}

bool find_shortest_path(int vid, int bottom_y, int voxelsize ,map<int,int>& pred , int& dest) {

	std::deque<int> queue;  
	unordered_set<int> visited;

	queue.push_back(vid);
	visited.insert(vid);

	while (!queue.empty()) {
		int voxel_id = queue.front();
		queue.pop_front();

		//cout << "queue.size = " << queue.size() << endl;
		vector<double> c = bmap[voxel_id].second;
		double cen[3] = {c[0],c[1],c[2]};
		// Check adjacent voxels and add them to the queue if not visited and not obstructed by mesh..
		vector<VoxelData> adj = adjacent(voxel_id, cen, voxelsize);

		for (int i = 0; i < adj.size(); i++) {
			//cout << "adjacentes are" << adj[i].voxelId << endl;
			if (visited.find(adj[i].voxelId) != visited.end()) continue;
			else
			{
				visited.insert(adj[i].voxelId);
				//pred[adj[i].voxelId] = voxel_id;
				if( pred.find(adj[i].voxelId) == pred.end()) pred.insert({adj[i].voxelId ,voxel_id });
				queue.push_back(adj[i].voxelId);

				//stopping bfs when we reach bottom floor....

				//vector<double> center = bmap[adj[i].voxelId].second;
				int ycord_now = adj[i].centerY;

				if (ycord_now == bottom_y) {
					dest = adj[i].voxelId;
					//cout << "destination is             " << dest<<endl;
				//	cout << "ycord_now =   " << ycord_now << endl;
					visited.clear();
					//cout << "pred ka size is" << pred.size();
					return true;
				}
			}
		}
	}

	return false;  // No path found
}



//v to pata hi nahi hai
//vector<int> BFS(int node, int bottom_y, int voxelsize)
//{
//	list<int> queue;
//	vector<bool> visited, pred, dist;
//
//
//	for (int i = 0; i < v; i++) {
//		//visited[i] = false;
//		dist[i] = INT_MAX;
//		pred[i] = -1;
//	}
//
//	visited[src] = true;
//	dist[src] = 0;
//	queue.push_back(src);
//
//	while (!queue.empty()) {
//		int u = queue.front();
//		queue.pop_front();
//		vector<double> cen = bmap[u].second;
//		double ucen[3] = {cen[0],cen[2],cen[3]};
//		vector<VoxelData> adj = adjacent(u, ucen, voxelsize);
//		
//		for (int i = 0; i < adj.size(); i++) {
//			if (visited[adj[i].voxelId] == false) {
//				visited[adj[i].voxelId] = true;
//				dist[adj[i].voxelId] = dist[u] + 1;
//				pred[adj[i].voxelId] = u;
//				queue.push_back(adj[i].voxelId);
//
//				//stopping bfs when we reach bottom floor....
//
//				vector<double> center = bmap[adj[i].voxelId].second;
//				int ycord_now = center[1];
//				if (ycord_now == bottom_y)	return true;
//			}
//		}
//	}
//
//	return false;
//}


int main()
{

	vtkNew<vtkNamedColors> colors;
	//std::string stl_file = "C:\\Users\\vipul.k\\Desktop\\Horn.stl";
	//// Create a point cloud
	//vtkNew<vtkSTLReader> reader;
	//reader->SetFileName(stl_file.c_str());
	//reader->Update();
	//vtkNew<vtkPolyDataMapper> pointsMapper;
	//pointsMapper->SetInputData(reader->GetOutput());
	//vtkNew<vtkActor> pointsActor;
	//pointsActor->SetMapper(pointsMapper);
	//pointsActor->GetProperty()->SetInterpolationToFlat();
	//pointsActor->GetProperty()->SetRepresentationToPoints();
	//pointsActor->GetProperty()->SetColor(colors->GetColor4d("Yellow").GetData());

	auto start_time = std::chrono::high_resolution_clock::now();

	std::string stl_file = "C:\\Users\\kunal.g\\Downloads\\cat.stl";

	// "D:\\DOWNLOADS\\cat.stl"  "D:\\DOWNLOADS\\cylinder.stl" "C:\\Users\\dell\\Documents\\Sample.stl"
	//  C:\Users\dell\Documents\Horn.stl
	// "D:\\DOWNLOADS\\Modular_Bar_and_Snap_Covid_Mask_Ear_Saver_4569503\\files\\14pbar_03.stl"
	// 
	// Read the STL file
	vtkNew<vtkSTLReader> reader;
	reader->SetFileName(stl_file.c_str());
	reader->Update();

	vtkPolyData* shape = reader->GetOutput();

	vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
	// Get the points (i.e. vertices)
	// Get the points (i.e. vertices)
	//vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

	//// Print out the coordinates of each vertex
	//for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
	//{
	//	double* p = points->GetPoint(i);
	//	std::cout << "Vertex " << i << ": (" << p[0] << ", " << p[1] << ", " << p[2] << ")" << std::endl;
	//}
	///////
	//
	//
	double c[3];
	shape->GetCenter(c);
	cout << "center   " << c[0] << " " << c[1] << " " << c[2] << endl;

	// Convert polygons to triangles
	vtkNew<vtkTriangleFilter> triangleFilter;
	triangleFilter->SetInputData(reader->GetOutput());
	triangleFilter->Update();



	// Create a mapper for the triangles
	vtkNew<vtkPolyDataMapper> triangleMapper;
	triangleMapper->SetInputData(triangleFilter->GetOutput());

	// Create an actor for the triangles
	vtkNew<vtkActor> triangleActor;
	triangleActor->SetMapper(triangleMapper);
	triangleActor->GetProperty()->SetInterpolationToFlat();
	triangleActor->GetProperty()->SetColor(colors->GetColor4d("Red").GetData());
	triangleActor->GetProperty()->SetOpacity(1);
	// previous opacity is 0.7


	// Step 2: Create a bounding box using vtkBoundingBox
	vtkBoundingBox boundingBox;
	boundingBox.SetBounds(shape->GetBounds());
	//
	// 
	// 
// 
	//maximum length in bounding box
	double length[3];
	boundingBox.GetLengths(length);
	cout << length[0] << " " << length[1] << " " << length[2] << endl;
	// Find the maximum side length among x, y, and z dimensions
	double maxlength = std::max(length[0], std::max(length[1], length[2]));
	cout << "maxlength" << maxlength << endl;
	boundingBox.Inflate((maxlength - length[0]) / 2, (maxlength - length[1]) / 2, (maxlength - length[2]) / 2);

	double ll[3];
	boundingBox.GetLengths(ll);
	cout << ll[0] << " " << ll[1] << " " << ll[2];
	double center[3];
	boundingBox.GetCenter(center);

	vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();
	//double bound[6];
	//boundingBox.GetBounds(bound);
	//cubeSource->SetBounds(bound);

	cubeSource->SetCenter(center);
	cubeSource->SetXLength(maxlength);
	cubeSource->SetYLength(maxlength);
	cubeSource->SetZLength(maxlength);
	cubeSource->Update();



	double center1[3];
	cubeSource->GetCenter(center1);
	cout << "center of cubesource " << center1[0] << " " << center1[1] << " " << center1[2] << endl;

	double cubebound[6] = { 0 };
	cubeSource->GetBounds(cubebound);

	//// Access the coordinates of the minimum and maximum points
	double xmin = cubebound[0];
	double ymin = cubebound[2];
	double zmin = cubebound[4];

	double xmax = cubebound[1];
	double ymax = cubebound[3];
	double zmax = cubebound[5];

	//// Use the coordinates (xmin, ymin, zmin) and (xmax, ymax, zmax) as needed
	std::cout << "Minimum point: (" << xmin << ", " << ymin << ", " << zmin << ")" << std::endl;
	std::cout << "Maximum point: (" << xmax << ", " << ymax << ", " << zmax << ")" << std::endl;


	//global variable
	minbox[0] = xmin;
	minbox[1] = ymin;
	minbox[2] = zmin;
	maxbox[0] = xmax;
	maxbox[1] = ymax;
	maxbox[2] = zmax;


	//enter the size of voxel
	cout << "enter the size of voxel" << endl;
	double voxelsize;
	cin >> voxelsize;

	int voxelid = 1;
	double cord[3] = { xmin + (voxelsize / 2) ,ymin + (voxelsize / 2),zmin + (voxelsize / 2) };
	double duplicate[3] = { xmin + (voxelsize / 2) ,ymin + (voxelsize / 2),zmin + (voxelsize / 2) };

	int dimension = maxlength / voxelsize;
	int insideoct = 0;
	int outsideoct = 0;
	int partialoct = 0;
	cout << "center" << cord[0] << " " << cord[1] << " " << cord[2] << endl;




	//cout << "dimenstion" << dimension;
	// 
	//       
	//first 25 voxel in same vertical 
	//first 5 voxel in same horizontal..
	//drawing for 60th voxel



	int no_of_points = 0;
	//checking for octants which are in bottom
	int bottomcheck = 0;
	//when to stop building the line when bottom reaached
	int stop = 0;
	//bottom floor y value (lowest y axis)..
	//int bottom_y = duplicate[1] + (voxelsize * dimension) - voxelsize;
	double bottom_y = duplicate[1];
	cout << "bottom y is" << bottom_y << endl;

	int bool_bottomcheck = 0;

	double center_y = ymin + (maxlength / 2);
	cout << "center y is " << center_y << endl;


	//max number of octants formed
	maxoct = (long long)dimension * (long long)dimension * (long long)dimension;
	cout << "maxoct  " << maxoct << endl;



	//setting default value of bmap
	//std::pair<char, std::vector<double>> defaultValue = { 'O', {0, 0, 0} };

	//// Inserting the default value for each key
	//for (int i = 0; i <= maxoct+1; ++i) {
	//	bmap[i] = defaultValue;
	//}

	double cordd[3] = { xmin + (voxelsize / 2) ,ymin + (voxelsize / 2),zmin + (voxelsize / 2) };


	for (int z = 0; z < dimension; z++) {
		cordd[2] = duplicate[2] + z * voxelsize;
		for (int y = 0; y < dimension; y++) {
			cordd[1] = duplicate[1] + y * voxelsize;
			for (int x = 0; x < dimension; x++) {
				cordd[0] = duplicate[0] + x * voxelsize;
				//cout << "voxelid " << voxelid << " cordinates  (" << cordd[0] << " " << cordd[1] << " " << cordd[2] << ") " << endl;
				// bmap default value set
				bmap[voxelid] = { 'O',{cordd[0],cordd[1],cordd[2]} };
				voxelid++;
			}
		}
	}



	double testcase[3];
	vtkIdType i = 0;
	//a single point which is inside the polydata... it get filled in testcase..
	polyData->GetPoint(i, testcase);
	cout << "testcase review dubug         mmmmmmmmmmmmmmmmmmmmmmmmmm " << endl;

	cout << testcase[0] << " " << testcase[1] << " " << testcase[2] << endl;
	//power problem
	//int dist = (sqrt((testcase[0] - xmin)* (testcase[0] - xmin) + (testcase[1] - ymin)* (testcase[1] - ymin) + (testcase[2] - zmin)* (testcase[2] - zmin)))/voxelsize;
	//check 2.......
	//finding the center process start
	double ix = (testcase[0] - duplicate[0]) / voxelsize;
	double iy = (testcase[1] - duplicate[1]) / voxelsize;
	double iz = (testcase[2] - duplicate[2]) / voxelsize;
	
	/*calculate the remainder
		rem = cord(x)-duplic(x) % v.size
		rem > vsize /2              ---->>>>    ceil(ix)
		rem < vsize/2				---->>>>    floor(ix)
		in case if rem == v.size/2 then point is on boundary take a lower
		or higher point in the boundary....
	*/

	cout << "testcase[0]           " << testcase[0] << endl;
	cout << "duplicate[0]          " << duplicate[0] << endl;
	cout << " voxelsize            " << voxelsize << endl;
	double remx = remainder((testcase[0] - duplicate[0]), voxelsize);
	//if (remx < 0) remx = voxelsize + remx;
	cout << "remainder of xxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << remx << endl;
	double remy = remainder((testcase[1] - duplicate[1]), voxelsize);
	cout << "remy is " << remy << endl;
	//if (remy < 0) remy = voxelsize + remy;
	cout << "remainder of xxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << remy << endl;
	double remz = remainder((testcase[2] - duplicate[2]), voxelsize);
	//if (remz < 0) remz = voxelsize + remz;
	cout << "remainder of xxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << remz << endl;
	/*if (remx > (voxelsize / 2)) ix = ceil(ix);
	else ix = floor(ix);
	if (remy > (voxelsize / 2)) iy = ceil(iy);
	else iy = floor(iy);
	if (remz > (voxelsize / 2)) iz = ceil(iz);
	else iz = floor(iz);*/
	if (remx > 0) ix = floor(ix);
	else ix = ceil(ix);
	if (remy > 0) iy = floor(iy);
	else iy = ceil(iy);
	if (remz > 0) iz = floor(iz);
	else iz = ceil(iz);
	double cen[3] = { duplicate[0] + ix * voxelsize,duplicate[1] + iy * voxelsize, duplicate[2] + iz * voxelsize };
	cout << "z cordinate is                  " << duplicate[2] << endl;
	cout << "ix   = " << ix << endl;
	cout << "iy     = " << iy << endl;
	cout << "iz        =" << iz << endl;
	cout << "the center is                   vvvvvvvvvvvvv" << endl;
	cout << cen[0] << "     " << cen[1] << "      " << cen[2] << endl;
	cout << "maxoctttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt" << maxoct << endl;
	//finding the start value minus the center..
	cout << "cen[0]-cord[0]                     =================" << cen[0] << endl;
	double start = (abs(cen[0] - cord[0]) * (1 / voxelsize)) + (abs(cen[1] - cord[1]) * (dimension / voxelsize)) + (abs(cen[2] - cord[2]) * (dimension / voxelsize) * (dimension)) + 1;
	cout << "startttttttttttttttttttttttttttttttttttttttttt" << start << endl;



	// dfs
	//n= h* l * b / v.size;
	cout << length[0] << " " << length[1] << " " << length[2] << endl;
	//visited[n]
	long long int len = ceil((length[0] * length[1] * length[2]) / (voxelsize * voxelsize * voxelsize));
	cout << "lennnnnnnnnnnnnnnnnnnnnnnn" << len << endl;
	len = maxoct + 1;
	//length of vis is problem
	vector<int> vis(len, 0);
	//start=center voxelid
	//adjacent nodes will be inserted through left right bottom top front back algo
	//will find the adjecent in the dfs loop only...
	vector<int> adj;
	//list of all inside voxels
	vector<int> insidels;
	//list of all partial voxels
	vector<int> partialls;
	//@@@@ 
	dfs(start, vis, insidels, partialls, voxelsize, shape);




	//int maxoct = insideoct + outsideoct + partialoct;


	int minus = std::cbrt(maxoct);
	//cout << "minus" << minus << endl;

	//array that stores all bottom octants voxel id..
	std::vector<int> bottom = getbottompartial(partialls);
	// making support for all bottom voxels....

	// A renderer and render window
	vtkNew<vtkRenderer> renderer;
	vtkNew<vtkRenderWindow> renderWindow;
	renderWindow->AddRenderer(renderer);


	// An interactor
	vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	renderWindowInteractor->SetRenderWindow(renderWindow);

	//now only fix this
	//support generation for all bottom voxels + deviation of shortest path
	cout << "bottom size" << bottom.size() << endl;


	for (int i = 0; i < bottom.size(); i++) {
		//straight bottome line
		vtkSmartPointer<vtkPoints> linepoints = vtkSmartPointer<vtkPoints>::New();
		//4 top vertex to center
		vtkSmartPointer<vtkPoints> centerline = vtkSmartPointer<vtkPoints>::New();

		//vid= voxel id of bottom partial voxel
		int vid = bottom[i];
		//cout <<"vid is =============================" << vid << " and vid position is equla to  === " << bmap[vid].first  << endl;
		cout << "vid is" << vid << endl;
		int no_of_points = 0;

		std::vector<double> cord = bmap[vid].second;
		double linep[3] = { cord[0],cord[1],cord[2] };

		//filltest(cord[0], cord[1], cord[2], linep);
		//traverse to add points in linepoints
		// j is voxelid
		//going down from single bottom partial voxel center

		int m = voxelsize / 2;
		double vertex1[3] = { cord[0] - m,cord[1] + m,cord[2] - m };
		double vertex2[3] = { cord[0] + m,cord[1] + m,cord[2] - m };
		double vertex3[3] = { cord[0] + m,cord[1] + m,cord[2] + m };
		double vertex4[3] = { cord[0] - m,cord[1] + m,cord[2] + m };

		centerline->InsertNextPoint(linep);    //point 0 
		centerline->InsertNextPoint(vertex1);  // point 1
		centerline->InsertNextPoint(vertex2);  // point 2
		centerline->InsertNextPoint(vertex3);  // point 3
		centerline->InsertNextPoint(vertex4);  // point 4

		//path for finding the linepoint  ###@

		map<int, int> pred;
		int dest = 0;

		find_shortest_path(vid, bottom_y, voxelsize, pred, dest);

		vector<int> path;
		int crawl = dest;
		
		linepoints->InsertNextPoint(linep);
		no_of_points++;
		//path.push_back(crawl);
		//cout << "destination is   " << dest << endl;
		path.push_back(dest);
		no_of_points++;

		while (pred.find(crawl) != pred.end()) {
			path.push_back(pred[crawl] - 1);
			crawl = pred[crawl];
			no_of_points++;
		}

		pred.clear();
		reverse(path.begin(), path.end());


		for (int i = 0; i < path.size(); i++) {
			cout << "index included" << path[i] << endl;
			vector<double> d = bmap[path[i]].second;
			double center[3] = { cord[0],cord[1],cord[2] };
			linepoints->InsertNextPoint(center);
		}

		
		//for (int j = vid; j >= 1; j -= minus) {


		//	//stop condition if bottom reached
		//	std::vector<double> cord = bmap[j].second;
		//	// 1 cond stop reached 2 cond outside oct excempt when j=vid p oct

		//	if (bmap[j].first == 'I' || bmap[j].first == 'P') {
		//		double c[3] = { cord[0],cord[1],cord[2] };
		//		int v = leftvoxel(j, c, voxelsize).voxelId;
		//		char pos = bmap[v].first;
		//		if (v != -100 && (pos == 'O')) {
		//			j = v;
		//			cout << "j in bottom here j = " << j << endl;
		//		}
		//		else {
		//			v = rightvoxel(j, c, voxelsize).voxelId;
		//			pos = bmap[v].first;
		//			if (v != 100 && (pos == 'O')) j = v;
		//			else {
		//				v = frontvoxel(j, c, voxelsize).voxelId;
		//				pos = bmap[v].first;
		//				if (v != 100 && (pos == 'O')) j = v;
		//				else {
		//					v = bottomvoxel(j, c, voxelsize).voxelId;
		//					pos = bmap[v].first;
		//					if (v != 100 && (pos == 'O')) j = v;
		//				}		
		//			}
		//		}
		//	}


		//	cout << "index include   :-" << j << endl;
		//	//common part even for other condition
		//	cord = bmap[j].second;
		//	double linep[3];
		//	//cout << j << endl;
		//	//cout << cord[0] << cord[1] << cord[2] << endl;
		//	filltest(cord[0], cord[1], cord[2], linep);
		//	linepoints->InsertNextPoint(linep);
		//	no_of_points++;
		//	if (cord[1] == bottom_y) break;
		//}


		vtkNew<vtkPolyLine> polyLine;
		polyLine->GetPointIds()->SetNumberOfIds(no_of_points);

		for (unsigned int i = 0; i < no_of_points; i++)
		{
			polyLine->GetPointIds()->SetId(i, i);
		}

		// Create a cell array to store the lines in and add the lines to it
		vtkNew<vtkCellArray> cells;
		cells->InsertNextCell(polyLine);

		// Create a polydata to store everything in
		vtkNew<vtkPolyData> linepolyData;

		// Add the points to the dataset
		linepolyData->SetPoints(linepoints);

		// Add the lines to the dataset
		linepolyData->SetLines(cells);

		// Setup actor and mapper
		vtkNew<vtkPolyDataMapper> linemapper;
		linemapper->SetInputData(linepolyData);

		vtkNew<vtkActor> lineactor;
		lineactor->SetMapper(linemapper);
		lineactor->GetProperty()->SetColor(colors->GetColor3d("black").GetData());
		int line_width = 2;  //# Change this value to the desired line width
		lineactor->GetProperty()->SetLineWidth(line_width);
		renderer->AddActor(lineactor);

		//center line top 4 vertex to center
		// Create a cell array to store the lines in and add the lines to it...				
		vtkNew<vtkCellArray> centercells;
		vtkIdType lineIndices01[2] = { 0, 1 }; // Connect Point 4 to Point 0
		centercells->InsertNextCell(2, lineIndices01);

		vtkIdType lineIndices02[2] = { 0, 2 };
		centercells->InsertNextCell(2, lineIndices02);

		vtkIdType lineIndices03[2] = { 0 ,3 };
		centercells->InsertNextCell(2, lineIndices03);

		vtkIdType lineIndices04[2] = { 0 , 4 };
		centercells->InsertNextCell(2, lineIndices04);

		// Create a polydata to store everything in
		vtkNew<vtkPolyData> centerlinepolyData;

		// Add the points to the dataset
		centerlinepolyData->SetPoints(centerline);

		// Add the lines to the dataset
		centerlinepolyData->SetLines(centercells);

		// Setup actor and mapper
		vtkNew<vtkPolyDataMapper> centerlinemapper;
		centerlinemapper->SetInputData(centerlinepolyData);

		vtkNew<vtkActor> centerlineactor;
		centerlineactor->SetMapper(centerlinemapper);
		centerlineactor->GetProperty()->SetColor(colors->GetColor3d("black").GetData());
		int centerline_width = 5;  //# Change this value to the desired line width
		centerlineactor->GetProperty()->SetLineWidth(centerline_width);
		renderer->AddActor(centerlineactor);
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Execution time: " << execution_time << " milliseconds" << std::endl;

	/*cout << "vxoelsizdfsfds" << "voxelsize" << endl;
	cout << "totalchildddddddddddddddddddddddddddddddd === ==== === " << totalchlild << endl;*/
	//drawing 60th voxel
	/*linepoints->InsertNextPoint(1.5, 0.5, 3.5);
	linepoints->InsertNextPoint(1.5, 1.5, 3.5);
	no_of_points = 2;*/


	// Create mapper and actor for the bounding box
	vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper1->SetInputData(cubeSource->GetOutput());
	vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->GetProperty()->SetOpacity(0.3);
	actor1->SetMapper(mapper1);


	cout << "a comp" << endl;
	// Add the actors to the scene 
	// ###$
	renderer->AddActor(triangleActor);
	//renderer->AddActor(octreeActor);
	//renderer->AddActor(stlactor);
	renderer->AddActor(actor1);
	/*renderer->AddActor(lineactor);
	renderer->AddActor(lineactor1);
	renderer->AddActor(lineactor2);
	renderer->AddActor(lineactor3);
	renderer->AddActor(lineactor4);*/
	renderer->SetBackground(colors->GetColor3d("white").GetData());
	//previous midnight blue
	cout << "b" << endl;

	// Render an image (lights and cameras are created automatically)
	renderWindow->SetWindowName("OctreeVisualize");
	renderWindow->SetSize(600, 600);
	renderWindow->Render();
	//cout << "c" << endl;
	//vtkNew<vtkSliderRepresentation2D> sliderRep;
	//sliderRep->SetMinimumValue(0);
	////sliderRep->SetMaximumValue(octree->GetLevel());
	//sliderRep->SetValue(0);
	//sliderRep->SetTitleText("Level");
	//sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	//sliderRep->GetPoint1Coordinate()->SetValue(.2, .2);
	//sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	//sliderRep->GetPoint2Coordinate()->SetValue(.8, .2);
	//sliderRep->SetSliderLength(0.075);
	//sliderRep->SetSliderWidth(0.05);
	//sliderRep->SetEndCapLength(0.05);
	//sliderRep->GetTitleProperty()->SetColor(
	//	colors->GetColor3d("Beige").GetData());
	//sliderRep->GetCapProperty()->SetColor(
	//	colors->GetColor3d("MistyRose").GetData());
	//sliderRep->GetSliderProperty()->SetColor(
	//	colors->GetColor3d("LightBlue").GetData());
	//sliderRep->GetSelectedProperty()->SetColor(
	//	colors->GetColor3d("Violet").GetData());



	//vtkNew<vtkSliderWidget> sliderWidget;
	//sliderWidget->SetInteractor(renderWindowInteractor);
	//sliderWidget->SetRepresentation(sliderRep);
	//sliderWidget->SetAnimationModeToAnimate();
	//sliderWidget->EnabledOn();


	//vtkNew<vtkSliderCallback> callback;
	////callback->Octree = octree;
	////callback->PolyData = polydata;
	//callback->Renderer = renderer;

	//sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	renderWindowInteractor->Start();
	return EXIT_SUCCESS;
}
