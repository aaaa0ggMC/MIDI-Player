/**@author aaaa0ggmc
 * @copyright  Copyright(c)2025
 * @date 2025/5/2
 * @description 模拟天体运动
 **/
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdint.h>
#include <unordered_map>
#include <cstdlib>
#include <cmath>
#include <random>
#include <fstream>
#include <cstring>
#include <iomanip>

///引力放缩倍数
constexpr float zoom_ratio = 1.0f;
///每一帧的模拟时间
constexpr float deltaT = 0.5;
constexpr int width = 800,height = 600;
//引力常数
constexpr float G = 6.67e-4 * zoom_ratio;
//碰撞恢复系数
constexpr float coll_e = 1.0f;
//最小距离
constexpr float min_dist = 0.1;
//QAQ sfml似乎对编译器运算支持不怎么好，这里使用constexpr会报错，算了，多个全局变量也不是不行
//小小破个戒
sf::Vector2f movVec = 0.008f * sf::Vector2f(static_cast<float>(width),static_cast<float>(height));

struct Planet{
	///星球的位置
	sf::Vector2f position {0,0};
	///星球速度
	sf::Vector2f velocity {0,0};
	///星球的Id
	int64_t id {0};
	///星球质量
	float mass {1};
	///记录
	sf::Vector2f oldpos {0,0};
	///颜色
	sf::Color color {255,255,255};
	///AddrionalAdd
	[[maybe_unused]] sf::Vector2f addiAdd {0,0};

	//direction a --> b
	static sf::Vector2f calcForce(const Planet& a,const Planet& b){
		constinit static float mdist2 = min_dist * min_dist;
		sf::Vector2f distance = b.oldpos - a.oldpos;
		float r_pow2 = distance.x * distance.x + distance.y * distance.y;
		if(r_pow2 <= mdist2){
			r_pow2 = mdist2;
		}
		float forceLength = G * a.mass * b.mass / r_pow2;
	
		return distance * (forceLength * std::sqrt(r_pow2));
	}

	float getRadius() const{
		return mass / 4.0f + 4;
	}

	void updateStatus(const std::unordered_map<int64_t,Planet> & planets){
		sf::Vector2f force {0,0};
		for(auto & [_,ct] : planets){
			if(ct.id == id)continue;
			/*sf::Vector2f dist = ct.oldpos - oldpos;
			float d_2 = dist.x * dist.x + dist.y * dist.y;
			float dp = ct.getRadius() + getRadius();
			if(d_2 <= std::pow(dp,2)){
				sf::Vector2f normalVec = dist / std::sqrt(d_2);
				float v1n = velocity.x * normalVec.x + velocity.y * normalVec.y;
				float v2n = ct.velocity.x * normalVec.x + ct.velocity.y * normalVec.y;

				float u1n = ( (mass - coll_e * ct.mass)*v1n + (1+coll_e)*v2n*ct.mass ) / ( mass + ct.mass);
			      	addiAdd = (u1n - v1n) * normalVec;	
			}else */force += calcForce(*this,ct);
		}
		
		//加速度
		force /= mass;

		//计算新的位置
		velocity += force * deltaT;
	}

	//二者分开计算防止每一颗星球计算时时间状态不一致
	void updatePos(){
		position += velocity * deltaT;
		oldpos = position;
	}

	void postUpdate(){
		//velocity += addiAdd;
		//addiAdd = {0,0};
		updatePos();
	}
};

int main(int argc,char * arg[]){
	int planet_count = (argc < 2)?(3):(std::atoi(arg[1]));
	int64_t planet_id_max = 0;
	std::unordered_map<int64_t,Planet> planets;
	sf::RenderWindow window (sf::VideoMode(width,height),"HelloNBodies.");
	sf::View myView (sf::FloatRect(0,0,width,height));

	window.setFramerateLimit(240);

	if(argc >= 3 && !std::strcmp(arg[1],"-file")){
		const char * fp = arg[2];
		char token;
		std::string tmp;
		std::ifstream ifs (fp);
		if(ifs.bad()){
			std::cerr << "读取文件失败！" << std::endl;
			return -1;
		}

		while(!ifs.eof()){
			//根据deepseek说会跳过空白
			ifs >> token;
			if(token == '#')getline(ifs,tmp);
			else if(token == 'p'){
				int r = 0,g =0,b=0;
				Planet p;
				p.id = ++planet_id_max;
				ifs >> p.position.x >> p.position.y >> p.velocity.x >> p.velocity.y >> p.mass;
				ifs >> r >> g >> b;
				p.color = {static_cast<sf::Uint8>(r),static_cast<sf::Uint8>(g),static_cast<sf::Uint8>(b)};
				p.oldpos = p.position;
				planets.insert(std::make_pair(p.id,p));	
			}
		}
		ifs.close();	
	}else{
		//随机数引擎
		std::mt19937 mt_engine (std::random_device{}());//梅森旋转
		std::uniform_int_distribution dist (0,width * height);//全局分布
	
		//init planets
		for(int64_t i = 0;i < planet_count;++i){
			std::uniform_int_distribution<unsigned int> ugen(100,255);
			std::uniform_real_distribution<float> velo (-2.0f,2.0f);
			Planet p;
			p.id = ++planet_id_max;
			p.position = {static_cast<float>(dist(mt_engine) % width),static_cast<float>(dist(mt_engine) % height)};
			p.mass = dist(mt_engine) %50 + 10;
			p.velocity = {velo(mt_engine),velo(mt_engine)};
			p.color = sf::Color(ugen(mt_engine),ugen(mt_engine),ugen(mt_engine));
			planets.insert(std::make_pair(p.id,p));
		}
	}

	std::cout << "Bodies:\n";
	std::cout << "| id |   x    |   y    | velocity_x | velocity_y |  mass  | r | g | b |\n";
	for(auto & [_,ct] : planets){
		std::cout << "|" <<  std::setw(4) << ct.id << "|" << std::setw(8) << std::setprecision(2) << ct.position.x <<  "|" <<
		       std::setw(8) << std::setprecision(2) << ct.position.y <<  "|" <<
 std::setw(12) << std::setprecision(4) << ct.velocity.x <<   "|" <<
std::setw(12) << std::setprecision(4) << ct.velocity.y <<   "|" <<
std::setw(8) << std::setprecision(2) << ct.mass <<  "|" <<

	std::setw(3) << static_cast<int>(ct.color.r) <<   "|" <<
std::setw(3) << static_cast<int>(ct.color.g) <<   "|" <<
std::setw(3) << static_cast<int>(ct.color.b) <<   "|" <<
std::endl; 
	}	

	//测试
	//planets.insert(std::make_pair(1,Planet({{400,300},{0,0},1,100,{400,300}})));
	//planets.insert(std::make_pair(2,Planet({{200,300},{0,1},2,10,{200,300}})));

	while(window.isOpen()){
		sf::Event event;
		while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed)window.close();
		}
		window.setView(myView);
		window.clear(sf::Color(0,0,0));
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			myView.move(0,-movVec.y);
		}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			myView.move(0,movVec.y);
		}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
			myView.move(-movVec.x,0);
		}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
			myView.move(movVec.x,0);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen)){
			myView.zoom(1.01f);
		}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Equal)){
			myView.zoom(1.0f / 1.01f);
		}

		//Draw phase
		for(auto & [_,ct] : planets){
			sf::CircleShape p;
			p.setRadius(ct.getRadius());
			p.setPosition(ct.position);
			p.setFillColor(ct.color);
			window.draw(p);
			//更新
			ct.updateStatus(planets);
		}
		for(auto & [_,ct] : planets){
			ct.postUpdate();
		}
		window.display();
	}
	return 0;
}

